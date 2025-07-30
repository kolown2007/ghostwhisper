# GhostWhisper

ESP32 ambient music generation system for QC Biennale 2025.

## Quick Start

1. **Configure WiFi**: Set credentials in `src/config/secrets.h`
2. **Connection Mode**: In `main.cpp`, choose:
   - `initializeConnection(ONLINE)` - Connect to WiFi network
   - `initializeConnection(OFFLINE)` - Create local AP at `192.168.4.1`
3. **Upload & Run**: Flash to ESP32 and access web UI at `ghostwhisper.local`

## main.cpp Essentials

The main initialization sequence:

```cpp
void setup() {
    // 1. Initialize hardware (I2S audio + SD card)
    initializeHardware();
    randomSeed(esp_random());
    
    // 2. Initialize connection (WiFi/AP + mDNS)
    initializeConnection(OFFLINE);  // Change to ONLINE for WiFi
    
    // 3. Start web control server
    initializeWebControl();
    
    // 4. Set default program mode
    setProgramMode(GENERATIVE_PROGRAM, "");
    
    Serial.println("Setup complete, starting program in loop...");
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();
    
    // CRITICAL: Audio processing must be first
    audio.loop();
    
    // Handle web server requests
    handleWebControl();
    
    // Handle program playback
    handleProgramPlayback();
    
    // Periodic debug and health monitoring
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}
```

## Program Modes

- **SHUFFLE**: Random SD card audio playback
- **GENERATIVE**: Algorithmic soundfont composition
- **STREAM**: Online audio streaming (ONLINE mode only)

## Hardware Pins

**I2S Audio**: DOUT=25, BCLK=27, LRC=26  
**SD Card**: MOSI=23, MISO=19, SCK=18, CS=5

## Access

- **URL**: `ghostwhisper.local` or device IP
- **WiFi Portal**: If connection fails, auto-creates config portal
- **Controls**: Volume, program selection, playback control via web UI