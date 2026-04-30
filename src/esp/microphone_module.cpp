#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

// ---- pin mapping ----
#define I2S_SCK_PIN 26
#define I2S_WS_PIN 25
#define I2S_SD_PIN 33

// ---- microphone settings ----
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("INMP441 microphone test started");

  // ---- I2S configuration ----
  i2s_config_t i2s_config = {};
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
  i2s_config.sample_rate = SAMPLE_RATE;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  i2s_config.dma_buf_count = 8;
  i2s_config.dma_buf_len = 256;
  i2s_config.use_apll = false;
  i2s_config.tx_desc_auto_clear = false;
  i2s_config.fixed_mclk = 0;

  // ---- I2S pin configuration ----
  i2s_pin_config_t pin_config = {};
  pin_config.bck_io_num = I2S_SCK_PIN;
  pin_config.ws_io_num = I2S_WS_PIN;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = I2S_SD_PIN;

  // ---- start I2S driver ----
  esp_err_t result = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  if (result != ESP_OK) {
    Serial.println("I2S driver installation failed");
    return;
  }

  result = i2s_set_pin(I2S_PORT, &pin_config);

  if (result != ESP_OK) {
    Serial.println("I2S pin configuration failed");
    return;
  }

  i2s_zero_dma_buffer(I2S_PORT);

  Serial.println("Speak or clap near the microphone");
}

void loop() {
  int32_t samples[BUFFER_SIZE];
  size_t bytesRead = 0;

  // ---- read microphone samples ----
  esp_err_t result = i2s_read(
    I2S_PORT,
    samples,
    sizeof(samples),
    &bytesRead,
    portMAX_DELAY
  );

  if (result != ESP_OK || bytesRead == 0) {
    Serial.println("No microphone data detected");
    delay(500);
    return;
  }

  int samplesRead = bytesRead / sizeof(int32_t);

  // ---- calculate average signal value ----
  double mean = 0;

  for (int i = 0; i < samplesRead; i++) {
    double value = samples[i] >> 8;
    mean += value;
  }

  mean /= samplesRead;

  // ---- calculate sound level ----
  double sum = 0;

  for (int i = 0; i < samplesRead; i++) {
    double value = (samples[i] >> 8) - mean;
    sum += value * value;
  }

  double rms = sqrt(sum / samplesRead);
  int volume = (rms - 7000) / 1000;

  if (volume < 0) {
    volume = 0;
  }

  if (volume > 10) {
    volume = 10;
  }

  // ---- print result ----
  Serial.print("Volume: ");
  Serial.println(volume);

  delay(100);
}