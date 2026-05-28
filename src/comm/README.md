# 🔁 Shared Communication Protocol

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

## 📦 Packet Envelope

`UDPPacket` contains:

- `crc32`
- `session_token`
- `sequence_number`
- `timestamp`
- `message_count`
- up to 64 `Message` entries

Minimum packet size is 17 bytes. Maximum packet size is 529 bytes.

## 🔐 Session Lifecycle

A new team should understand the session flow before debugging WiFi:

1. Python sends a packet with session token `0`.
2. ESP32 treats token `0` as a session request.
3. ESP32 generates a random session token.
4. ESP32 replies with an acknowledgement packet.
5. Python stores the returned session token.
6. Python sends later packets with that token and increasing sequence numbers.
7. ESP32 rejects packets with an invalid token or old sequence number.
8. ESP32 clears the session after a timeout if no valid packets arrive.

## ✉️ Message Format

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

## 🧰 Implemented Helpers

`message.h` currently provides helpers for:

- `create_move_by`
- `create_control_begin`
- `create_acknowledge`
- `create_sensor_distance`
- `init_packet`
- `append_message`
- `finalize_packet_crc`
- `validate_packet`

## 🤖 Behavior Kinds

The current behavior enum is shared by `src/comm/message.h` and `src/control/pluto_server/message.py`:

| Behavior | Current firmware handling |
| --- | --- |
| `BEHAVIOR_FLIP` | Accepted over UDP and logged, but the UDP handler does not yet start a flip motion |
| `BEHAVIOR_BOW` | Starts the bow motion |
| `BEHAVIOR_SIT` | Maps to stop/stand as a safe placeholder |
| `BEHAVIOR_GIVE_PAW` | Starts the paw motion |

## ➕ Adding Or Changing A Message

The protocol is shared by firmware and Python. When adding a message:

1. Add the enum value in `src/comm/message.h`.
2. Add the same enum value in `src/control/pluto_server/message.py`.
3. Decide whether the 32-bit payload is raw data, two signed 16-bit values, an enum, or unused.
4. Add a helper constructor in both languages if the message will be sent often.
5. Update packet parsing or switch handling in `src/esp/main.cpp`.
6. Update UI, speech, or controller logic that creates the message.
7. Document the message in [SOFTWARE_WIFI.md](../../SOFTWARE_WIFI.md).

Do not change the size of `Message` or `UDPPacket` without updating both languages and the documentation.

## 📚 Related Documentation

- [WiFi Protocol](../../SOFTWARE_WIFI.md)
- [ESP32 Firmware](../esp/README.md)
- [Python Controller](../control/README.md)
