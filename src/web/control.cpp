/**
 * @file control.cpp
 * @brief Web server control interface for GhostWhisper system.
 * @details Provides HTTP endpoints for volume control, playback management, and system monitoring.
 */

#include "control.h"
#include "web_routes.h"
#include "../hardware/volume_control.h"
#include "../managers/meme_manager.h"
#include "web_utils.h"
#include "../config/config.h"
#include "../managers/connection_manager.h"
#include <WiFi.h>

// External web server reference
extern WebServer server;

/**
 * @brief Initialize the web server control interface.
 */
void initializeWebControl() {
    // Check if WiFi is connected or in AP mode
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) {
        Serial.println("WiFi not connected and not in AP mode. Web server not started.");
        return;
    }

    // Initialize subsystems
    initializeVolumeControl();
    
    // Start the web server
    if (startWebServer()) {
        // Scan meme files at startup
        scanMemeFiles();
        
        // Check memory usage and limits
        checkMemoryLimits();
        
        Serial.println("Web control interface initialized successfully!");
    } else {
        Serial.println("Failed to start web server!");
    }
}

/**
 * @brief Handle incoming web server requests.
 */
void handleWebControl() {
    if (isWebServerActive()) {
        server.handleClient();
    }
}
