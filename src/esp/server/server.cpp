#include <server/server.h>
#include <Arduino.h>

namespace pluto
{
  PlutoServer::PlutoServer(uint16_t port)
      : _port(port)
  {
  }

  void PlutoServer::begin()
  {
    _msgQueue = xQueueCreate(QUEUE_SIZE, sizeof(Message));

    xTaskCreatePinnedToCore(
        PlutoServer::serverTask, "NetTask", 8192, this, 1, nullptr,
        0 // Core 0
    );
  }

  void PlutoServer::addAP(const char* ssid, const char* pass)
  {
    _wifiMulti.addAP(ssid, pass);
  }

  bool PlutoServer::getNextMessage(Message& msg)
  {
    return xQueueReceive(_msgQueue, &msg, 0) == pdTRUE;
  }

  void PlutoServer::serverTask(void* pvParameters)
  {
    PlutoServer* server = static_cast<PlutoServer*>(pvParameters);

    while (true)
    {
      if (server->_wifiMulti.run() == WL_CONNECTED)
      {
        server->_udp.begin(server->_port);
        server->handleIncomingPackets();
      }
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }

  void PlutoServer::handleIncomingPackets()
  {
    int packetSize = _udp.parsePacket();
    if (packetSize < 17)
      return;

    UDPPacket packet;
    int read = _udp.read(reinterpret_cast<uint8_t*>(&packet), sizeof(UDPPacket));

    if (!validate_packet(packet, read))
      return;

    if (!_hasSession)
    {
      if (packet.session_token == SESSION_REQUEST_TOKEN)
      {
        _sessionToken = esp_random();
        _hasSession   = true;
        _lastSequence = packet.sequence_number;
        sendAcknowledge(packet.sequence_number);
      }
      return;
    }

    if (packet.session_token != _sessionToken
        || packet.sequence_number <= _lastSequence)
      return;

    _lastSequence = packet.sequence_number;
    for (uint8_t i = 0; i < packet.message_count; ++i)
      xQueueSend(_msgQueue, &packet.messages[i], 0);

    sendAcknowledge(packet.sequence_number);
  }

  void PlutoServer::sendAcknowledge(uint32_t sequence)
  {
    UDPPacket ackPacket;
    init_packet(ackPacket, _sessionToken, sequence, millis());

    Message ackMsg = create_acknowledge(sequence);
    append_message(ackPacket, ackMsg);
    finalize_packet_crc(ackPacket);

    _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
    _udp.write(
        reinterpret_cast<uint8_t*>(&ackPacket), 17 + (8 * ackPacket.message_count));
    _udp.endPacket();
  }
} // namespace pluto