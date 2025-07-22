#include "hardware_setup.h"
#include "config.h"
#include <SPI.h>
#include <SD.h>
#include <esp_task_wdt.h>
#include <esp_system.h>

// Remove duplicate pin definitions - now in config.h
Audio audio;

void initializeHardware() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) {
        ; // Wait for Serial to initialize
    }

    Serial.println("=== Hardware Initialization ===");

    // Initialize SD card with SPI pins
    Serial.println("Initializing SD card...");
    delay(1000); // Longer delay for SD card to stabilize
    
    // Try multiple initialization attempts
    bool sdInitialized = false;
    for (int attempt = 1; attempt <= 3; attempt++) {
        Serial.printf("SD init attempt %d/3...\n", attempt);
        
        if (SD.begin(SD_CS)) {
            sdInitialized = true;
            Serial.println("SD card initialization successful!");
            break;
        } else {
            Serial.printf("Attempt %d failed, retrying...\n", attempt);
            delay(1000);
        }
    }
    
    if (!sdInitialized) {
        Serial.println("SD card initialization failed after 3 attempts!");
        Serial.println("Hardware troubleshooting:");
        Serial.println("1. Check ALL connections:");
        Serial.println("   CS   -> Pin 5");
        Serial.println("   MOSI -> Pin 23");
        Serial.println("   MISO -> Pin 19"); 
        Serial.println("   SCK  -> Pin 18");
        Serial.println("2. Power supply:");
        Serial.println("   VCC -> 3.3V (NOT 5V!)");
        Serial.println("   GND -> Ground");
        Serial.println("3. SD card format: FAT32");
        Serial.println("4. SD card size: ≤32GB");
        Serial.println("Trying to format SD card...");
        // Note: ESP32 SD library doesn't have format function
        // Card must be formatted on computer as FAT32
        Serial.println("Continuing without SD card...");
    } else {
        // Test SD card functionality
        Serial.println("Testing SD card read/write...");
        File testFile = SD.open("/test.txt", FILE_WRITE);
        if (testFile) {
            testFile.println("GhostWhisper SD Test");
            testFile.close();
            Serial.println("SD card test write successful.");
            
            // Try reading back
            testFile = SD.open("/test.txt");
            if (testFile) {
                Serial.println("SD card test read successful:");
                while (testFile.available()) {
                    Serial.write(testFile.read());
                }
                testFile.close();
            }
        } else {
            Serial.println("SD card write test failed - card may be read-only or corrupted.");
        }
    }

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
