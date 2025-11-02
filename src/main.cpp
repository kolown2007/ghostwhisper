
#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "managers/connection_manager.h"
#include "managers/debug_manager.h"
#include "secrets.h"
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "managers/mqtt_manager.h"





void setup() {
    // Basic serial + watchdog setup
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) { ; }

    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    configTime(0, 0, "pool.ntp.org", "time.google.com");

    // Initialize hardware (audio object, SD optional)
    initializeHardware();
    audio.setVolume(DEFAULT_VOLUME);

    // Initialize network (online-only connection manager)
    Serial.println("Initializing network connection...");
    initializeConnection(ONLINE);

    // If Wi‑Fi connected, play an MP3 from a URL once
    {
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start) < 10000) {
            delay(100);
        }
        if (WiFi.status() == WL_CONNECTED) {
            const char* url = "https://kolown.net/audiolib/bluetooth_pair.mp3"; // TODO: replace with your MP3 URL
            if (!audio.connecttohost(url)) {
                Serial.println("Failed to start MP3 playback");
            }
        } else {
            Serial.println("Wi‑Fi not connected; skipping initial MP3");
        }

  // Initialize MQTT manager after network is available
  mqttManager_begin();

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

    // Let the MQTT manager handle reconnects and incoming messages
    mqttManager_loop();

    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}

