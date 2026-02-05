# Websockets - Examples

This folder contains examples of communication via Websocket protocol. Read more about Websockets here: <https://en.wikipedia.org/wiki/WebSocket>

For websockets, you need a server that handles the communication. Check the example folders on how to create a server either with Python or with ESP32

  - [01_Server_Python](./01_Server_Python/): Runs the server on a laptop using Python. Follow the extra steps required for the Python Websockets server in the [README](./01_Server_Python/README.md)
  - [02_Server_ESP32](./02_Server_ESP32/): Runs a server on a ESP32. Follow extra steps required for ESP32 Websockets server in the [README](./02_Server_ESP32_LightBulbs/README.md)

## Sofware installation

1. You need to have Arduino IDE configured for ESP32 as described in [this link](<https://learn.sparkfun.com/tutorials/installing-board-definitions-in-the-arduino-ide/installing-a-third-party-board-definition).
   - Preferences > Settings > Additional Boars Manager URLSs
   - Paste in the field the URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. This project uses the implementation of Arduino libraries. Go to Tools > Manage Libraries. Search and install `WebSockets` by *`Markus Sattler`*. Read more here: <https://github.com/Links2004/arduinoWebSockets>
   - Example **02_Server_ESP32** also requires installing by *`Khoi Hoang`*. Read more here: <https://docs.arduino.cc/libraries/wifiwebserver> and the specific [README]
   - Example **03_Client_Example** also requires installing `Adafruit NeoPixel` by *`Adafruit`*. Read more here: <https://github.com/adafruit/Adafruit_NeoPixel>

