#include "hardware_setup.h"
#include "../config/config.h"
#include <esp_task_wdt.h>
#include <esp_system.h>


// Remove duplicate pin definitions - now in config.h
Audio audio;

// Blink built-in LED (usually pin 2 on ESP32)
void blinkBuiltinLED(int times, int delayMs) {
    const int BUILTIN_LED = 2;
    pinMode(BUILTIN_LED, OUTPUT);
    for (int i = 0; i < times; ++i) {
        digitalWrite(BUILTIN_LED, HIGH);
        delay(delayMs);
        digitalWrite(BUILTIN_LED, LOW);
        delay(delayMs);
    }
}



void initializeHardware() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) {
        ; // Wait for Serial to initialize
    }

    // Blink built-in LED to indicate startup
    blinkBuiltinLED(1, 200);

    Serial.println("=== Hardware Initialization ===");
    Serial.println("SD card initialization skipped (not used in this system)");

    // Initialize audio with I2S pinout
    Serial.println("Initializing audio...");
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(DEFAULT_VOLUME); // Set safer default volume
    Serial.println("Audio initialized.");

    // Memory and stability improvements
    Serial.println("=== System Information ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("CPU frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
    Serial.println("===========================");
    
    // Enable watchdog timer for stability
    esp_task_wdt_init(WATCHDOG_TIMEOUT_SEC, true); // 30 second timeout
    esp_task_wdt_add(NULL); // Add current task to watchdog
    Serial.println("Watchdog timer enabled (30s timeout)");
}
