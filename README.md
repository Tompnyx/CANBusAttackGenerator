# CANBusAttackGenerator

### General Information and Acknowledgements

The following project has been created to perform attacks on a CAN bus, typical of those present in a vehicle. These attacks include:

- Impersonation,
- Fuzzing,
- Replay,
- Denial-of-Service (DOS), and
- Frame-Drop attacks.

Inspiration to format the outputted messages is taken from [HCRL](https://ocslab.hksecurity.net/welcome)'s work on their CAN Dataset for intrusion detection ([OTIDS](https://ocslab.hksecurity.net/Dataset/CAN-intrusion-dataset)). It is formatted in the following way:
```
Timestamp:  [Time since initialised in milliseconds]    ID: [Arbitration or CAN ID] DLC:    [Length of the data package sent]   [The data package sent]
```

### How to run

The code in this project was developed to run on the [Arduino Uno Rev3](https://store.arduino.cc/products/arduino-uno-rev3) and the [CAN-BUS Shield V2.0](https://wiki.dfrobot.com/CAN-BUS_Shield_V2__SKU__DFR0370_) (which runs on the [MCP2515 Chip](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf)), but any Arduino and CAN-BUS shield that supports the required libraries should work too.

The libraries used in this project include:

- [Seeed Arduino CAN](https://github.com/Seeed-Studio/Seeed_Arduino_CAN) Version 2.3.3

To run on your Arduino, make sure you have set the:

- baud,
- SPI_CS pin, and
- CAN_INT pin.

To perform the different functionality of the program, five different operational modes have been created. These modes are named after the attacks they perform, and can be switched between via setting the **op** parameter to the different names.

- **Impersonation** creates a CAN message crafted around an Electronic Control Unit (ECU) to send periodically (This CAN message needs to be hard coded in the **impersonation()** function)
- **Fuzzing** generates a random CAN message to send periodically
- **Replay** periodically listens to the CAN bus until a message is received, then resends that message a set number of times (Determined by the **numRepeat** variable located in the **replay()** function)
- **DoS** periodically sends an empty CAN message with a high arbitration value (low CAN ID number)
- **FrameDrop** injects a message simultaneously with another ECU, exploiting a vulnerability in the arbitration phase of the CAN protocol 

Note that the **DoS** and **FrameDrop** modes are partially implemented as of now. More information can be found in the [ITU-T's Guidelines for an intrusion detection system for in-vehicle networks](https://www.itu.int/rec/T-REC-X.1375-202010-I).

### Development Environment
The IDE [CLion](https://www.jetbrains.com/clion/) was used with the plugins [PlatformIO for CLion](https://plugins.jetbrains.com/plugin/13922-platformio-for-clion) and [Serial Port Monitor](https://plugins.jetbrains.com/plugin/8031-serial-port-monitor).