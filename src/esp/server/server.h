#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <comm/message.h> // pluto's messages

namespace pluto
{
  class PlutoServer
  {
  public:
    PlutoServer(uint16_t port);

    void begin();
    void addAP(const char* ssid, const char* pass);
    bool getNextMessage(Message& msg);

  private:
    static void serverTask(void* pvParameters);
    void handleIncomingPackets();
    void sendAcknowledge(uint32_t sequence);

    uint16_t _port;
    WiFiMulti _wifiMulti;
    WiFiUDP _udp;
    QueueHandle_t _msgQueue;

    uint32_t _sessionToken = 0;
    uint32_t _lastSequence = 0;
    bool _hasSession       = false;

    static constexpr size_t QUEUE_SIZE              = 256;
    static constexpr uint32_t SESSION_REQUEST_TOKEN = 0x0;
  };
} // namespace pluto