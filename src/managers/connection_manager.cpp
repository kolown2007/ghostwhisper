/**
 * @file connection_manager.cpp
 * @brief Manages WiFi connection (ONLINE only)
 */

#include "connection_manager.h"
#include "../config/config.h"
#include "secrets.h" // WiFi credentials (WIFI_ssid / WIFI_password)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp_task_wdt.h>

// Global connection state (force online mode)
ConnectionMode currentConnectionMode = ONLINE;
bool wifiConnected = false;

/**
 * @brief Initialize connection in ONLINE mode only
 */
void initializeConnection(ConnectionMode mode) {
    (void)mode; // ignore requested mode; system is full online mode
    currentConnectionMode = ONLINE;

    delay(1000);
    Serial.println("");
    Serial.println("==================================================");
    Serial.println("=== ONLINE MODE (streaming only) ===");

    // Status LED
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    // Ensure watchdog is enabled for runtime
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);

    // Start WiFi station mode and connect using credentials from secrets.h
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);

    Serial.print("Connecting to WiFi SSID: ");
    Serial.println(String(WIFI_ssid));

    WiFi.begin(WIFI_ssid, WIFI_password);

    // Wait for connection (timeout ~15 seconds)
    int attempts = 0;
    const int maxAttempts = 30;
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print('.');
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        setConnectionStatusLED(true);

        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("Access web interface at: http://" + WiFi.localIP().toString());

        // Start mDNS responder for discovery (non-blocking)
        if (MDNS.begin("ghostwhisper")) {
            Serial.println("mDNS responder started");
            MDNS.addService("http", "tcp", 80);
            Serial.println("Also accessible at: http://ghostwhisper.local");
        } else {
            Serial.println("mDNS failed to start - use IP address only");
        }
    } else {
        wifiConnected = false;
        setConnectionStatusLED(false);
        Serial.println("Failed to connect to WiFi (timeout).");
    }

    Serial.println("==================================================");
    Serial.println("Connection initialization complete.");
}

/**
 * @brief Check if the device is connected to WiFi
 */
bool isOnline() {
    return (currentConnectionMode == ONLINE && wifiConnected);
}

/**
 * @brief Get the current connection mode
 */
ConnectionMode getConnectionMode() {
    return currentConnectionMode;
}

/**
 * @brief Set the connection mode (no-op: system is online-only)
 */
void setConnectionMode(ConnectionMode mode) {
    // System is configured as online-only. Allow changing the variable for API compatibility.
    currentConnectionMode = (mode == ONLINE) ? ONLINE : ONLINE;
}

/**
 * @brief Set the connection status LED
 */
void setConnectionStatusLED(bool connected) {
    if (connected) {
        digitalWrite(STATUS_LED_PIN, HIGH); // solid ON
    } else {
        digitalWrite(STATUS_LED_PIN, LOW);
    }
}

/**
 * @brief Turn off all status LEDs
 */
void turnOffAllLEDs() {
    digitalWrite(STATUS_LED_PIN, LOW);
}

/**
 * @brief Reset WiFi settings - no-op in online-only build
 */
void resetWiFiSettings() {
    Serial.println("resetWiFiSettings() called - no-op in online-only build");
}

/**
 * @brief Start WiFi config portal - no-op in online-only build
 */
void startWiFiConfigPortal() {
    Serial.println("startWiFiConfigPortal() called - no-op in online-only build");
}

/**
 * @brief Clear WiFi credentials - no-op in online-only build
 */
void clearWiFiCredentials() {
    Serial.println("clearWiFiCredentials() called - no-op in online-only build");
}
