#pragma once

#include <Arduino.h>
#include <driver/i2s.h>
#include <freertos/ringbuf.h>
#include <cstring>

namespace pluto
{
  struct MicState
  {
    RingbufHandle_t ring_buffer      = nullptr;
    TaskHandle_t dma_task            = nullptr;
    volatile bool is_listening       = false;
    volatile uint32_t current_energy = 0;
  };

  template<
      uint8_t SCK_PIN, uint8_t WS_PIN, uint8_t SD_PIN, i2s_port_t PORT = I2S_NUM_0>
  class SensorMicrophone
  {
  private:
    static MicState state;

    // 16kHz, mono
    static constexpr int SAMPLE_RATE = 16000;
    static constexpr int DMA_BUF_LEN = 512;

    // An 8KB ring buffer holds about 250ms of 16-bit audio.
    static constexpr int RING_BUF_SIZE = 1024 * 8;

    static void IRAM_ATTR dma_reader_task(void* arg)
    {
      (void)arg;

      int32_t sample_buffer[DMA_BUF_LEN];
      size_t bytes_read = 0;

      while (state.is_listening)
      {
        // block until I2S has data
        i2s_read(
            PORT, sample_buffer, sizeof(sample_buffer), &bytes_read, portMAX_DELAY);

        int samples_read = bytes_read / sizeof(int32_t);

        if (samples_read <= 0)
        {
          continue;
        }

        // downsample inplace
        int16_t* in_place_samples = (int16_t*)sample_buffer;
        uint64_t sum_of_squares   = 0;

        // downsample to 16-bit and multiply by 4 ( >> 16 * 4 = >> 14)
        for (int i = 0; i < samples_read; i++)
        {
          int16_t sample      = (int16_t)(sample_buffer[i] >> 14);
          in_place_samples[i] = sample;
          sum_of_squares += sample * sample;
        }

        state.current_energy = sum_of_squares / samples_read;

        // wait max 10 ticks if full, then drop data
        xRingbufferSend(
            state.ring_buffer,
            in_place_samples,
            samples_read * sizeof(int16_t),
            pdMS_TO_TICKS(10));
      }

      vTaskDelete(NULL);
    }

  public:
    SensorMicrophone() = default;

    bool begin() noexcept
    {
      state.ring_buffer = xRingbufferCreate(RING_BUF_SIZE, RINGBUF_TYPE_BYTEBUF);
      if (!state.ring_buffer)
      {
        return false;
      }

      i2s_config_t i2s_config = {
          .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
          .sample_rate          = SAMPLE_RATE,
          .bits_per_sample      = I2S_BITS_PER_SAMPLE_32BIT,
          .channel_format       = I2S_CHANNEL_FMT_ONLY_RIGHT,
          .communication_format = I2S_COMM_FORMAT_STAND_I2S,
          .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
          .dma_buf_count        = 8,
          .dma_buf_len          = DMA_BUF_LEN,
          .use_apll             = false};

      i2s_pin_config_t pin_config = {
          .bck_io_num   = SCK_PIN,
          .ws_io_num    = WS_PIN,
          .data_out_num = I2S_PIN_NO_CHANGE,
          .data_in_num  = SD_PIN};

      if (i2s_driver_install(PORT, &i2s_config, 0, NULL) != ESP_OK)
      {
        return false;
      }

      if (i2s_set_pin(PORT, &pin_config) != ESP_OK)
      {
        return false;
      }

      state.is_listening = true;

      xTaskCreatePinnedToCore(
          dma_reader_task,
          "mic_dma_task",
          4096,
          NULL,
          1,
          &state.dma_task,
          0
        );

      return true;
    }

    size_t read_frame(int16_t* out_buffer, size_t requested_bytes) const noexcept
    {
      size_t bytes_received = 0;

      void* data = xRingbufferReceive(state.ring_buffer, &bytes_received, 0);

      if (data)
      {
        size_t bytes_to_copy =
            (bytes_received < requested_bytes) ? bytes_received : requested_bytes;

        memcpy(out_buffer, data, bytes_to_copy);
        vRingbufferReturnItem(state.ring_buffer, data);

        return bytes_to_copy;
      }

      return 0;
    }

    uint32_t current_energy() const noexcept
    {
      return state.current_energy;
    }
  };

  template<uint8_t S, uint8_t W, uint8_t D, i2s_port_t P>
  MicState SensorMicrophone<S, W, D, P>::state;
} // namespace pluto