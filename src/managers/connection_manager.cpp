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
    
    // Give serial monitor time to connect and ensure Serial is ready
    delay(3000);
    Serial.println(""); // Print blank line for clarity
    Serial.println("==================================================");
    
    // Initialize status LED
    pinMode(STATUS_LED_PIN, OUTPUT);
    
    // Turn off LED initially
    digitalWrite(STATUS_LED_PIN, LOW);
    
    if (mode == ONLINE) {
        Serial.println("=== ONLINE MODE ===");
        Serial.println("Connecting to existing WiFi network...");
        
        WiFiManager wifiManager;
        
        // Clear WiFi credentials if requested
        if (CLEAR_WIFI_ON_STARTUP) {
            Serial.println("Clearing stored WiFi credentials...");
            wifiManager.resetSettings();
        }
        
        // Set timeout for configuration mode
        wifiManager.setConfigPortalTimeout(WIFI_TIMEOUT_SEC);
        
        // Try to connect with saved credentials, or start config portal
        if (!wifiManager.autoConnect(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD)) {
            Serial.println("Failed to connect to existing WiFi.");
            Serial.println("Starting WiFiManager configuration portal...");
            Serial.println("Connect to: " + String(WIFI_SSID_NAME) + " (Password: " + String(OFFLINE_AP_PASSWORD) + ")");
            Serial.println("Then open: http://ghostwhisper.local to configure WiFi");
            
            wifiConnected = true;
            setConnectionStatusLED(false);
        } else {
            Serial.println("WiFi connected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            Serial.println("Access web interface at: http://" + WiFi.localIP().toString());
            wifiConnected = true;
            setConnectionStatusLED(true);
            
            // Initialize mDNS
            if (MDNS.begin("ghostwhisper")) {
                Serial.println("mDNS responder started");
                MDNS.addService("http", "tcp", 80);
                Serial.println("Also accessible at: http://ghostwhisper.local");
            } else {
                Serial.println("mDNS failed to start - use IP address only");
            }
        }
    } else {
        Serial.println("=== OFFLINE MODE ===");
        Serial.println("Creating secure WiFi Access Point...");
        
        // Create Access Point
        WiFi.mode(WIFI_AP);
        WiFi.disconnect(true);
        delay(100);
        
        // Create secure Access Point
        Serial.println("Attempting softAP with:");
        Serial.println("  SSID length: " + String(strlen(WIFI_SSID_NAME)));
        Serial.println("  Password length: " + String(strlen(OFFLINE_AP_PASSWORD)));
        Serial.println("  Password: '" + String(OFFLINE_AP_PASSWORD) + "'");
        
        bool apStarted = WiFi.softAP(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD, 1, 0, 4);
        
        if (!apStarted) {
            // Try simpler method if first fails
            apStarted = WiFi.softAP(WIFI_SSID_NAME, OFFLINE_AP_PASSWORD);
        }
        
        if (apStarted) {
            Serial.println("Secure Access Point created: " + String(WIFI_SSID_NAME));
            Serial.println("Password: " + String(OFFLINE_AP_PASSWORD));
            Serial.println("IP: " + WiFi.softAPIP().toString());
            Serial.println("Web interface: http://192.168.4.1");
            
            // Initialize mDNS for Access Point mode
            WiFi.softAPsetHostname("ghostwhisper");
            delay(100);
            if (MDNS.begin("ghostwhisper")) {
                Serial.println("mDNS responder started");
                MDNS.addService("http", "tcp", 80);
                Serial.println("Also accessible at: http://ghostwhisper.local");
            } else {
                Serial.println("mDNS failed - use IP address only");
            }
            
            wifiConnected = true;
            setConnectionStatusLED(true);
        } else {
            Serial.println("Failed to create Access Point");
            wifiConnected = false;
            setConnectionStatusLED(false);
        }
    }
    
    Serial.println("==================================================");
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
