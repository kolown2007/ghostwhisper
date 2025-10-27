
#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "secrets.h"
#include "managers/debug_manager.h"

#include <WiFi.h>
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

    // Connect to WiFi using credentials in secrets.h
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_ssid, WIFI_password);

    unsigned long start = millis();
    const unsigned long wifiTimeout = 20000; // 20s
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < wifiTimeout) {
        delay(200);
        Serial.print('.');
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected: " + WiFi.localIP().toString());
    } else {
        Serial.println("WiFi not connected - continuing (local playback may still work)");
    }

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

    // Occasional debug/health logging
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}

