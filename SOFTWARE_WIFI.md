## WiFi Protocol:
Pluto and its controller communicate by WiFi using the User Datagram Protocol (UDP). This was done for performance as Pluto is closer to a real-time system.

A custom binary format (defined in `src/comm/message.h` and mirrored in `src/control/pluto_server/message.py`) is used for messages.

## Connecting the ESP to the WiFi:
In `src/esp/main.cpp`, in the `setup` function, call `PLUTO_SERVER.addAP(<WIFI_NAME>, <WIFI_PASSWORD>)`. Multiple calls to `addAP` can be done, the ESP automatically tries to connect to any available one.

In `src/control/main.py`, when constructing `PlutoController()`, pass the IP of the ESP.

The ESP uses port `4242` by default.