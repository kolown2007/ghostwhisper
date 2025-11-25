## Firmware Upload


1. go to this website: https://esptool.spacehuhn.com/ , insert your esp32 and press connect
2. download the necessary [firmwares](/firmware), it should have 4 files
    - 0x1000 → bootloader.bin
    - 0x8000 → partitions.bin
    - 0xE000 → boot_app0.bin
    - 0x10000 → firmware.bin
3. upload the firmwares

note: if you want to upload the source code via VScode we have also instructions [here](/artwork-manual/firmware-upload-vscode.md)

-------

## GhostWhisper Assembly Links:

**1. Assemble the Hardware**: [wiring-guide](/artwork-manual/wiring-guide.md)

**2. Install the Firmware**: *you are here*

**3. Connect the ESP32 to WiFi**: [connecting to wifi](/artwork-manual/wifi-connect.md)