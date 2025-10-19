# GhostWhisper
![GhostWhisper Logo](assets/ghostwhisper.png)

Short description: a simplified online-mode build of GhostWhisper that streams audio and serves files from a remote server (no SD card). All device logic is moved to a separate server.

Contributors: KoloWn and Roan Alvarez

Note: this repository is a work in progress — the codebase is being cleaned. Do not copy or deploy as-is.

## Online mode

In online mode the ESP32 connects to WiFi and streams audio from a remote server. Local storage (SD card) is not used.


## Wiring guide (I2S)

Connect the I2S amplifier to the ESP32 using the following pins (example mapping):

```text
DOUT -> GPIO25
BCLK -> GPIO27
LRC  -> GPIO26
3.3V / 5V -> VCC (match amplifier voltage)
GND -> GND
```

Adjust pin numbers in code if you use different ESP32 boards or custom wiring.

## Usage / Instructions

### Artwork description
WIP

### Medium
- Speaker units, 
- amplifier, 
- ESP32 microcontroller, 
- WiFi network, 
- and custom software on a remote server.

### List of components (per unit)

- 1 × NodeMCU-32s (ESP32)
- 1 × MAX98357 (I2S 3W amplifier)
- 1 set × Speakers
	- Option A: two × 4Ω 3W full-range speakers
	- Option B: one × 8Ω 20W horn speaker
- 1 × 5V power supply (match current requirements)
- 1 set × Dupont jumper wires (≈30 cm)

### Other important tools

- Micro-USB or USB-C cable for uploading code to the microcontroller
- Computer with VS Code and PlatformIO IDE installed
- Soldering tools (soldering iron and solder) / jumper wires

---


