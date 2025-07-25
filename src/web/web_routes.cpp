/**
 * @file web_routes.cpp
 * @brief Web server route definitions and setup implementation
 */

#include "web_routes.h"
#include "http_handlers.h"
#include "../config/config.h"
#include <WiFi.h>

// Web server instance
WebServer server(WEB_SERVER_PORT);
static bool webServerActive = false;

void setupWebRoutes() {
    // Basic pages
    server.on("/", handleRoot);
    server.on("/meme", handleMemePage);
    server.on("/status", handleStatus);
    
    // Volume control endpoints
    server.on("/volume/up", handleVolumeUp);
    server.on("/volume/down", handleVolumeDown);
    server.on("/volume/set", handleSetVolume);
    server.on("/test", handleVolumeTest);
    
    // Playback control endpoints
    server.on("/random", handleRandomPlay);
    server.on("/stop", handleStop);
    server.on("/pause", handlePause);
    server.on("/resume", handleResume);
    
    // System endpoints
    server.on("/memory", handleMemoryCheck);
    server.on("/wifi/reset", handleWiFiReset);
    server.on("/wifi/config", handleWiFiConfig);
    
    // Radio program endpoints
    server.on("/program/shuffle", handleProgramShuffle);
    server.on("/program/generative", handleProgramGenerative);
    server.on("/program/stream", handleProgramStream);
    server.on("/program/newstream", handleProgramNewStream);
    server.on("/program/status", handleProgramStatus);
    
    // Program-specific endpoints
    server.on("/shuffle/next", handleShuffleNext);
    server.on("/shuffle/folder", handleShuffleFolder);
    server.on("/generative/sequence", handleGenerativeSequence);
    server.on("/generative/regenerate", handleGenerativeRegenerate);
    server.on("/stream/connect", handleStreamConnect);
    server.on("/stream/reset", handleStreamReset);
    
    // Meme soundboard endpoints
    server.on("/meme/list", handleMemeList);
    server.on("/meme/play", handleMemePlay);
    
    // Static file endpoints
    server.on("/style.css", handleStaticFile);
    server.on("/app.js", handleStaticFile);
    server.on("/favicon.ico", handleStaticFile);
    
    // 404 handler
    server.onNotFound(handleNotFound);
}

bool startWebServer() {
    // Check if WiFi is connected or in AP mode
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) {
        Serial.println("WiFi not connected and not in AP mode. Web server not started.");
        return false;
    }
    
    // Setup routes
    setupWebRoutes();
    
    // Start the server
    server.begin();
    webServerActive = true;
    
    return true;
}

void stopWebServer() {
    if (webServerActive) {
        server.stop();
        webServerActive = false;
        Serial.println("Web server stopped.");
    }
}

bool isWebServerActive() {
    return webServerActive;
}

String getWebServerStatus() {
    if (webServerActive) {
        return "Web server active at " + WiFi.localIP().toString() + ":80";
    } else {
        return "Web server inactive";
    }
}
