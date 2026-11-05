# Two-Node STM32 Environmental and Vibration Monitoring System

A professional embedded IoT project featuring two communicating STM32 microcontrollers that collect environmental and vibration data, display readings on an OLED, and upload measurements to a cloud dashboard.

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        CLOUD (ThingSpeak)                          │
└──────────────────────────────┬──────────────────────────────────────┘
                               │ HTTP/AT Commands
                        ┌──────┴──────┐
                        │   ESP-01    │
                        │  (Wi-Fi)    │
                        └──────┬──────┘
                               │ UART
┌──────────────────────────────┴──────────────────────────────────────┐
│                    GATEWAY NODE (NUCLEO-F446RE)                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────────┐   │
│  │ FreeRTOS │  │ UART RX  │  │ Data     │  │ SSD1306 OLED     │   │
│  │ Kernel   │  │ Task     │  │ Process  │  │ Display Task     │   │
│  └──────────┘  └────┬─────┘  │ Task     │  └──────────────────┘   │
│                     │        └──────────┘                          │
└─────────────────────┼──────────────────────────────────────────────┘
                      │ UART (CRC-16 Packets)
┌─────────────────────┼──────────────────────────────────────────────┐
│                 SENSOR NODE (STM32F411)                             │
│  ┌──────────┐  ┌────┴─────┐  ┌──────────┐  ┌──────────────────┐   │
│  │ BME280   │  │ UART TX  │  │ MPU6050  │  │ Packet Builder   │   │
│  │ Temp/Hum │  │          │  │ Accel/   │  │ + CRC-16         │   │
│  │ /Press   │  │          │  │ Gyro     │  │                  │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────────────┘   │
└────────────────────────────────────────────────────────────────────┘
```

## Features

- **Environmental monitoring:** Temperature, humidity, atmospheric pressure (BME280)
- **Vibration monitoring:** 3-axis accelerometer with RMS calculation (MPU6050)
- **Reliable communication:** UART with CRC-16 packet validation
- **Real-time OS:** FreeRTOS on gateway with task-based architecture
- **Local display:** SSD1306 OLED showing live sensor data
- **Cloud connectivity:** ESP-01 Wi-Fi module uploading to ThingSpeak
- **Error handling:** Communication recovery, sensor fault detection, Wi-Fi reconnection

## Hardware

| Component | Role |
|-----------|------|
| STM32F411 (BlackPill) | Sensor node — reads BME280 and MPU6050, transmits packets |
| NUCLEO-F446RE | Gateway node — receives packets, displays on OLED, uploads to cloud |
| BME280 | Temperature, humidity, pressure sensor (I2C) |
| MPU6050 | 3-axis accelerometer/gyroscope (I2C) |
| SSD1306 | 128×64 OLED display (I2C) |
| ESP-01 | ESP8266 Wi-Fi module for cloud connectivity (UART) |

## Project Structure

```
├── sensor-node/          # STM32F411 firmware
│   ├── Core/
│   │   ├── Inc/          # Header files
│   │   └── Src/          # Source files
│   └── Drivers/
│       ├── BME280/       # Environmental sensor driver
│       ├── MPU6050/      # Accelerometer driver
│       └── Protocol/     # UART packet protocol + CRC
│
├── gateway-node/         # NUCLEO-F446RE firmware
│   ├── Core/
│   │   ├── Inc/
│   │   └── Src/
│   └── Drivers/
│       ├── SSD1306/      # OLED display driver
│       ├── ESP8266/      # Wi-Fi AT command driver
│       └── Protocol/     # UART packet protocol + CRC
│
└── docs/                 # Documentation, wiring, test results
```

## Development Timeline

| Day | Milestone | Status |
|-----|-----------|--------|
| 1 | STM32F411 initialization, clock, UART, GPIO | ✅ |
| 2 | BME280 I2C driver | ⬜ |
| 3 | MPU6050 vibration monitoring | ⬜ |
| 4 | Two-node UART communication + CRC-16 | ⬜ |
| 5 | Gateway FreeRTOS integration | ⬜ |
| 6 | SSD1306 OLED display | ⬜ |
| 7 | ESP-01 Wi-Fi driver | ⬜ |
| 8 | Cloud dashboard integration | ⬜ |
| 9 | System integration and testing | ⬜ |
| 10 | Documentation and release | ⬜ |

## Building

### Prerequisites
- STM32CubeIDE (v1.13+)
- STM32F4 HAL Library
- ST-Link drivers

### Compilation
1. Open STM32CubeIDE
2. Import existing project: `sensor-node/` or `gateway-node/`
3. Build: Project → Build Project (Ctrl+B)
4. Flash: Run → Run As → STM32 C/C++ Application

## License

MIT License
