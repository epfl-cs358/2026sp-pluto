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
    _txQueue  = xQueueCreate(QUEUE_SIZE, sizeof(Message));

    xTaskCreatePinnedToCore(
        PlutoServer::serverTask, "NetTask", 8192, this, 1, nullptr, 0);
  }

  void PlutoServer::addAP(const char* ssid, const char* pass)
  {
    _wifiMulti.addAP(ssid, pass);
  }

  bool PlutoServer::getNextMessage(Message& msg)
  {
    return xQueueReceive(_msgQueue, &msg, 0) == pdTRUE;
  }

  void PlutoServer::sendMessage(const Message& msg)
  {
    if (_hasSession)
    {
      xQueueSend(_txQueue, &msg, 0);
    }
  }

  void PlutoServer::serverTask(void* pvParameters)
  {
    PlutoServer* server = static_cast<PlutoServer*>(pvParameters);
    bool is_listening   = false;

    while (true)
    {
      if (server->_wifiMulti.run() == WL_CONNECTED)
      {
        if (!is_listening)
        {
          server->_udp.begin(server->_port);
          if (!MDNS.begin("PlutoController"))
          {
            Serial.println("Error setting up MDNS responder!");
          }
          else
          {
            MDNS.addService("pluto", "udp", server->_port);
            Serial.println("mDNS responder started successfully.");
          }
          is_listening = true;
        }

        if (server->_hasSession)
        {
          if (millis() - server->_lastPacketTime > SESSION_TIMEOUT_MS)
          {
            server->_hasSession   = false;
            server->_sessionToken = 0;
          }
          else
          {
            server->sendOutboundPackets();
          }
        }

        server->handleIncomingPackets();
      }
      else
      {
        if (is_listening)
        {
          MDNS.end();
          server->_udp.stop();
          is_listening        = false;
          server->_hasSession = false;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(5));
    }
  }

  void PlutoServer::handleIncomingPackets()
  {
    int packetSize = _udp.parsePacket();
    if (packetSize < 17)
      return;

    UDPPacket packet = {};
    int read = _udp.read(reinterpret_cast<uint8_t*>(&packet), sizeof(UDPPacket));

    if (!validate_packet(packet, read))
      return;

    if (packet.session_token == SESSION_REQUEST_TOKEN)
    {
      _sessionToken   = esp_random();
      _hasSession     = true;
      _lastSequence   = packet.sequence_number;
      _lastPacketTime = millis();
      _clientIP       = _udp.remoteIP();
      _clientPort     = _udp.remotePort();

      sendAcknowledge(packet.sequence_number);
      return;
    }

    int32_t diff =
        static_cast<int32_t>(packet.sequence_number - _lastSequence.load());
    if (!_hasSession || packet.session_token != _sessionToken || diff <= 0)
      return;

    _lastSequence   = packet.sequence_number;
    _lastPacketTime = millis();
    _clientIP       = _udp.remoteIP();
    _clientPort     = _udp.remotePort();

    for (uint8_t i = 0; i < packet.message_count; ++i)
    {
      if (xQueueSend(_msgQueue, &packet.messages[i], 0) != pdTRUE)
        break;
    }

    sendAcknowledge(packet.sequence_number);
  }

  void PlutoServer::sendOutboundPackets()
  {
    Message msg;
    if (xQueueReceive(_txQueue, &msg, 0) != pdTRUE)
      return;

    UDPPacket txPacket = {};
    uint32_t nextSeq   = (_lastSequence.load() + 1) & 0xFFFFFFFF;
    _lastSequence.store(nextSeq);

    init_packet(txPacket, _sessionToken, nextSeq, millis());
    append_message(txPacket, msg);

    while (txPacket.message_count < MAX_MESSAGES_PER_PACKET
           && xQueueReceive(_txQueue, &msg, 0) == pdTRUE)
    {
      append_message(txPacket, msg);
    }

    finalize_packet_crc(txPacket);

    _udp.beginPacket(_clientIP, _clientPort);
    _udp.write(
        reinterpret_cast<uint8_t*>(&txPacket), 17 + (8 * txPacket.message_count));
    _udp.endPacket();
  }

  void PlutoServer::sendAcknowledge(uint32_t sequence)
  {
    UDPPacket ackPacket = {};
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