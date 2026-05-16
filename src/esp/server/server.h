#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <atomic>

#include <comm/message.h>

namespace pluto
{
  class PlutoServer
  {
  public:
    PlutoServer(uint16_t port);

    void begin();
    void addAP(const char* ssid, const char* pass);
    bool getNextMessage(Message& msg);
    void sendMessage(const Message& msg);

  private:
    static void serverTask(void* pvParameters);
    void handleIncomingPackets();
    void sendOutboundPackets();
    void sendAcknowledge(uint32_t sequence);

    uint16_t _port;
    WiFiMulti _wifiMulti;
    WiFiUDP _udp;
    QueueHandle_t _msgQueue;
    QueueHandle_t _txQueue;

    IPAddress _clientIP;
    uint16_t _clientPort{0};

    std::atomic<uint32_t> _sessionToken{0};
    std::atomic<uint32_t> _lastSequence{0};
    std::atomic<uint32_t> _lastPacketTime{0};
    std::atomic<bool> _hasSession{false};

    static constexpr size_t QUEUE_SIZE              = 256;
    static constexpr uint32_t SESSION_REQUEST_TOKEN = 0x0;
    static constexpr uint32_t SESSION_TIMEOUT_MS    = 10000;
  };
} // namespace pluto