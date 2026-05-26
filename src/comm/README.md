# Shared Communication Protocol

This directory contains the C++ protocol definitions shared by the ESP32 firmware and mirrored by the Python controller.

Primary file:

```text
src/comm/message.h
```

Python mirror:

```text
src/control/pluto_server/message.py
```

These two files must stay in sync.

## Packet Envelope

`UDPPacket` contains:

- `crc32`
- `session_token`
- `sequence_number`
- `timestamp`
- `message_count`
- up to 64 `Message` entries

Minimum packet size is 17 bytes. Maximum packet size is 529 bytes.

## Message Format

Each `Message` is exactly 8 bytes:

- 2-bit family
- 4-bit kind
- 26-bit event clock or duration field
- 32-bit payload union

Message families:

- `KIND_INFO`
- `KIND_MOVE`
- `KIND_SENSOR`
- `KIND_BEHAVIOR`

## Implemented Helpers

`message.h` currently provides helpers for:

- `create_move_by`
- `create_control_begin`
- `create_acknowledge`
- `create_sensor_distance`
- `init_packet`
- `append_message`
- `finalize_packet_crc`
- `validate_packet`

## Related Documentation

- [WiFi Protocol](../../SOFTWARE_WIFI.md)
- [ESP32 Firmware](../esp/README.md)
- [Python Controller](../control/README.md)
