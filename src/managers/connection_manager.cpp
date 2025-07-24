/**
 * @file connection_manager.cpp
 * @brief Manages WiFi connection modes (ONLINE/OFFLINE)
 */

#include "connection_manager.h"
#include "../config/config.h"
#include "../config/secrets.h" // For WiFi credentials
#include <WiFiManager.h>
#include <WiFi.h>
#include <ESPmDNS.h>

// Global connection state
ConnectionMode currentConnectionMode = DEFAULT_CONNECTION_MODE; // Use config default
bool wifiConnected = false;

/**
 * @brief Initialize connection based on the specified mode
 */
void initializeConnection(ConnectionMode mode) {
    currentConnectionMode = mode;
    
    // Initialize status LED
    pinMode(STATUS_LED_PIN, OUTPUT);
    
    // Turn off LED initially
    digitalWrite(STATUS_LED_PIN, LOW);
    
    if (mode == ONLINE) {
        Serial.println("=== ONLINE MODE ===");
        Serial.println("Initializing WiFi...");
        
        WiFiManager wifiManager;
        
        // Clear WiFi credentials if requested
        if (CLEAR_WIFI_ON_STARTUP) {
            Serial.println("Clearing stored WiFi credentials...");
            wifiManager.resetSettings();
        }
        
        // Set timeout for configuration mode
        wifiManager.setConfigPortalTimeout(WIFI_TIMEOUT_SEC); // 3 minutes
        
        // Try to connect with saved credentials, or start config portal
        if (!wifiManager.autoConnect(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD)) {
            Serial.println("Failed to connect to WiFi.");
            Serial.println("Starting WiFiManager configuration portal...");
            Serial.println("Connect to WiFi network: " + String(WIFI_SSID_NAME));
            Serial.println("Password: " + String(OFFLINE_AP_PASSWORD));
            Serial.println("Then open: http://ghostwhisper.local to configure WiFi");
            
            // WiFiManager will have created an AP for configuration
            wifiConnected = true; // Consider connected for portal access
            setConnectionStatusLED(false); // Red LED for configuration mode
        } else {
            Serial.println("WiFi connected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            wifiConnected = true;
            setConnectionStatusLED(true); // Blue LED for successful connection
            
            // Initialize mDNS
            if (MDNS.begin("ghostwhisper")) {
                Serial.println("mDNS responder started");
                Serial.println("Device accessible at: http://ghostwhisper.local");
            } else {
                Serial.println("Error setting up mDNS responder!");
            }
        }
    } else {
        Serial.println("=== OFFLINE MODE ===");
        Serial.println("Creating WiFi Access Point for local web interface...");
        
        // Create Access Point
        WiFi.mode(WIFI_AP);
        bool apStarted = WiFi.softAP(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD); // Use config password
        
        if (apStarted) {
            Serial.println("Access Point created successfully!");
            Serial.print("AP IP address: ");
            Serial.println(WiFi.softAPIP());
            Serial.println("Connect to WiFi network: " + String(WIFI_SSID_NAME));
            Serial.println("Password: " + String(OFFLINE_AP_PASSWORD));
            Serial.println("Then open: http://192.168.4.1 in your browser");
            Serial.println("Or open: http://ghostwhisper.local in your browser");
            
            // Initialize mDNS for Access Point mode
            if (MDNS.begin("ghostwhisper")) {
                Serial.println("mDNS responder started for AP mode");
            } else {
                Serial.println("Error setting up mDNS responder for AP mode!");
            }
            
            wifiConnected = true; // Set as connected for AP mode
            setConnectionStatusLED(true); // Blue LED for AP mode
        } else {
            Serial.println("Failed to create Access Point");
            wifiConnected = false;
            setConnectionStatusLED(false); // Red LED for failure
        }
    }
    
    Serial.println("Connection initialization complete.");
}

/**
 * @brief Check if the device is connected to WiFi or serving as AP
 */
bool isOnline() {
    return (currentConnectionMode == ONLINE && wifiConnected) || 
           (currentConnectionMode == OFFLINE && wifiConnected);
}

/**
 * @brief Get the current connection mode
 */
ConnectionMode getConnectionMode() {
    return currentConnectionMode;
}

/**
 * @brief Set the connection mode
 */
void setConnectionMode(ConnectionMode mode) {
    currentConnectionMode = mode;
}

/**
 * @brief Set the connection status LED
 */
void setConnectionStatusLED(bool connected) {
    if (connected) {
        // Connected: LED ON (solid)
        digitalWrite(STATUS_LED_PIN, HIGH);
    } else {
        // Not connected: LED OFF (we could add blinking later)
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
 * @brief Reset WiFi settings and force configuration portal
 */
void resetWiFiSettings() {
    Serial.println("=== RESETTING WIFI SETTINGS ===");
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    Serial.println("WiFi credentials cleared. Restarting...");
    delay(1000);
    ESP.restart();
}

/**
 * @brief Start WiFiManager configuration portal on demand
 */
void startWiFiConfigPortal() {
    Serial.println("=== STARTING WIFI CONFIG PORTAL ===");
    WiFiManager wifiManager;
    
    // Set timeout for configuration mode
    wifiManager.setConfigPortalTimeout(WIFI_TIMEOUT_SEC);
    
    // Start configuration portal
    if (wifiManager.startConfigPortal(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD)) {
        Serial.println("WiFi configured successfully via config portal!");
        Serial.print("New IP address: ");
        Serial.println(WiFi.localIP());
        wifiConnected = true;
        setConnectionStatusLED(true);
        
        // Initialize mDNS after successful configuration
        if (MDNS.begin("ghostwhisper")) {
            Serial.println("mDNS responder started");
            Serial.println("Device accessible at: http://ghostwhisper.local");
        } else {
            Serial.println("Error setting up mDNS responder!");
        }
    } else {
        Serial.println("Config portal timed out or failed");
        wifiConnected = false;
        setConnectionStatusLED(false);
    }
}

/**
 * @brief Clear WiFi credentials stored on the ESP32
 */
void clearWiFiCredentials() {
    Serial.println("=== CLEARING WIFI CREDENTIALS ===");
    WiFi.disconnect(true, true); // Erase WiFi credentials
    delay(1000);
    Serial.println("WiFi credentials cleared. Restarting...");
    ESP.restart();
}
