## WiFi Protocol:
Pluto and its controller communicate by WiFi using the User Datagram Protocol (UDP). This was done for performance as Pluto is closer to a real-time system.

A custom binary format (defined in `src/comm/message.h` and mirrored in `src/control/pluto_server/message.py`) is used for messages.

## Connecting the ESP to the WiFi:
In `src/esp/main.cpp`, in the `setup` function, call `PLUTO_SERVER.addAP(<WIFI_NAME>, <WIFI_PASSWORD>)`. Multiple calls to `addAP` can be done, the ESP automatically tries to connect to any available one.

In `src/control/main.py`, when constructing `PlutoController()`, pass the IP of the ESP.

The ESP uses port `4242` by default.

## Binary Format:
To minimize network overhead, Pluto uses a custom, tightly packed binary format (`#pragma pack(1)`). Instead of transmitting single commands, up to 64 messages are batched into a single UDP packet.

### Packet Structure:
A packet consists of a 17-byte header followed by a variable-length message array. This keeps the maximum packet size at 529 bytes, preventing UDP fragmentation.

- Header: Contains a CRC32 checksum for data integrity, a session token, a sequence number for message acknowledgments, and the device timestamp.
- Message Array: Contains between 0 and 64 individual messages.

### Message Structure:
Every message is strictly 8 bytes (64 bits) long, split into a 32-bit bitfield header and a 32-bit payload union.

```text
  Bit 0   2      6                                32
      +---+------+---------------------------------+
Header|Fam| Kind |     event_clock_or_duration     |
      | 2 |  4   |             26 bits             |
      +---+------+---------------------------------+
      |                                            |
      |                  payload                   |
      |                  32 bits                   |
      +--------------------------------------------+

```

- Header (4 bytes): Defines the message category (`family`), the specific command (`kind`), and a 26-bit field used for lease durations or sensor timestamps.
- Payload (4 bytes): A union containing the actual data. Its interpretation depends on the header. For example, a `MOVE_BY` message interprets these bytes as two 16-bit directional integers, while a `SENSOR_DISTANCE` message reads it as a 32-bit unsigned integer.