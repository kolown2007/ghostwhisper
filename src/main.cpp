
#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "managers/connection_manager.h"
#include "managers/debug_manager.h"
#include <esp_task_wdt.h>

// Replace this URL with your HTTPS MP3 stream
const char* STREAM_URL = "https://kolown.net/storage/projects/whisper/Dayang%20Dayang.mp3";

void setup() {
    // Basic serial + watchdog setup
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) { ; }

    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    Serial.println("=== GhostWhisper - minimal stream player ===");

    // Initialize hardware (audio object, SD optional)
    initializeHardware();
    audio.setVolume(DEFAULT_VOLUME);

    // Initialize network (online-only connection manager)
    Serial.println("Initializing network connection...");
    initializeConnection(ONLINE);

    // Attempt to connect to HTTPS stream
    Serial.println("Attempting to connect to stream: ");
    Serial.println(STREAM_URL);

    if (audio.connecttohost(STREAM_URL)) {
        Serial.println("Stream started successfully");
    } else {
        Serial.println("Failed to start stream");
    }

    // Reduce watchdog timeout for normal operation
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();

    // Drive audio processing
    audio.loop();

    // Auto-reconnect stream if it stops and we're online
    static unsigned long lastStreamAttempt = 0;
    const unsigned long streamRetryInterval = 5000; // ms
    if (!audio.isRunning() && isOnline() && (millis() - lastStreamAttempt > streamRetryInterval)) {
        Serial.println("Stream stopped — attempting reconnect...");
        if (audio.connecttohost(STREAM_URL)) {
            Serial.println("Reconnected to stream");
        } else {
            Serial.println("Reconnect failed, will retry...");
        }
        lastStreamAttempt = millis();
    }

    // Occasional debug/health logging
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}

