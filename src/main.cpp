
#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "managers/connection_manager.h"
#include "managers/debug_manager.h"
#include "secrets.h"
#include <esp_task_wdt.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// API URL is defined in secrets.h as GHOSTWHISPER_API_URL

String fetchStreamUrl(const char* apiUrl) {
    WiFiClientSecure client;
    client.setInsecure(); // skip certificate validation (change if you have CA)

    HTTPClient http;
    Serial.print("Fetching stream URL from API: ");
    Serial.println(apiUrl);

    if (!http.begin(client, apiUrl)) {
        Serial.println("HTTP begin failed");
        return String();
    }

    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, code=%d\n", code);
        http.end();
        return String();
    }

    // Parse JSON directly from the stream
    // Using JsonDocument instead of deprecated DynamicJsonDocument
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, http.getStream());
    http.end();

    if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        return String();
    }

    const char* url = doc["url"];
    if (!url) return String();
    return String(url);
}

// Current stream URL used for connect/reconnect
String currentStreamUrl;

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

    // Fetch stream URL from API (constant in secrets.h)
    String streamUrl = fetchStreamUrl(GHOSTWHISPER_API_URL);
    if (streamUrl.length() == 0) {
        Serial.println("No stream URL received from API; not attempting playback.");
        currentStreamUrl = String();
    } else {
        Serial.println("Attempting to connect to stream: ");
        Serial.println(streamUrl);
        // store current stream URL for reconnects
        currentStreamUrl = streamUrl;

        if (audio.connecttohost(currentStreamUrl.c_str())) {
            Serial.println("Stream started successfully");
        } else {
            Serial.println("Failed to start stream");
        }
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
        if (audio.connecttohost(currentStreamUrl.c_str())) {
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

