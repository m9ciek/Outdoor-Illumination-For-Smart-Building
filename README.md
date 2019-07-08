## Outdoor illumination module for smart building system with CAN bus, based on Arduino UNO board.


## Requirements
- 3x Arduino: https://amzn.to/2YFDwrb
- 3x WS2812B LED strip: https://amzn.to/2Xyewpm
- 3x Can Bus Module MCP2515: https://amzn.to/32dKrKp

## How it works

Device was mainly built to show, that CAN Bus can be used not only in cars but in smart building projects like this one. 

This project uses the [FastLED](https://github.com/FastLED/FastLED) and [Seeed Studio](https://github.com/Seeed-Studio/CAN_BUS_Shield) – Can Bus Shield library to handle most of the background job processing. The device is split into three modules, connected with CAN bus. Each one has 7 programmed illumination effects, changing every 10 seconds by sending signal from the main Arduino board, which is the main module responsible for controlling second and third element.

MCP2515 module is fit into PCB board with extra buzzer and LED diode for testing purposes. It also can be upgraded by adding any external wireless signal recievier (i.e. Bluetooth, Wi-Fi) to manage the device remotely.
