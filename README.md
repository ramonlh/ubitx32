# ubitx32

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/ramonlh/ubitx32)](https://github.com/ramonlh/ubitx32/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/ramonlh/ubitx32)](https://github.com/ramonlh/ubitx32/issues)

Firmware and interface for the uBITx transceiver (and derivatives)  
Maintained by: Ramon LH  

---

## Description

**ubitx32** is a modular, open-source firmware project designed to enhance the uBITx transceiver experience. It provides a modern graphical interface, easy control, and extended functionality beyond the original firmware.

Features include real-time frequency display, mode selection, power control, and optional advanced functions depending on your hardware.

![ubitx32 Interface Example](docs/images/interface_example.png)

---

## Key Features

- Graphical interface showing frequency, mode, and status.  
- Support for OLED/LCD displays and multiple encoder/button inputs.  
- Basic transceiver functions: tuning, mode selection, and power adjustments.  
- Modular firmware architecture for easy customization.  
- Open-source and community-driven.

---

## Hardware Requirements

| Component              | Notes                                       |
|------------------------|--------------------------------------------|
| Microcontroller        | ESP32, STM32, or other supported boards    |
| Transceiver            | uBITx or compatible clone                   |
| Display                | OLED or LCD (configurable in firmware)      |
| Controls               | Rotary encoder, push buttons                |
| Power Supply           | 5V–12V depending on board                   |
| Connections            | Audio output, optional USB for programming |

**Example Wiring Diagram:**  

![ubitx32 Wiring](docs/images/wiring_diagram.png)

---

## Software Requirements

- C/C++ Compiler (GCC or compatible)  
- PlatformIO, Arduino IDE, or Makefile environment  
- Basic knowledge of flashing firmware onto microcontrollers

---

## Installation / Compilation

1. Clone the repository:

```bash
git clone https://github.com/ramonlh/ubitx32.git
cd ubitx32
