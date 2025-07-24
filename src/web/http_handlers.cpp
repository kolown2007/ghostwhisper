/**
 * @file http_handlers.cpp
 * @brief HTTP request handlers implementation
 */

#include "http_handlers.h"
#include "../config/config.h"
#include "../hardware/hardware_setup.h"
#include "web_routes.h"
#include "../hardware/volume_control.h"
#include "../managers/meme_manager.h"
#include "web_utils.h"
#include "../managers/radio_manager.h"
#include "../managers/connection_manager.h"
#include "../managers/stream_manager.h"
#include "../managers/shuffle_manager.h"
#include "../managers/generative_manager.h"
#include "../config/musicdata.h"
#include <WiFi.h>
#include <SD.h>
#include <WebServer.h>

extern WebServer server;

// Basic page handlers
void handleRoot() {
    server.send(200, "text/html", generateMainPage());
}

void handleMemePage() {
    server.send(200, "text/html", loadHTMLFromSD("/meme.html"));
}

void handleStatus() {
    server.send(200, "application/json", generateStatusJson());
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    
    Serial.println("404 Error: " + message);
    server.send(404, "text/plain", message);
}

void handleStaticFile() {
    String path = server.uri();
    if (path.endsWith("/")) path += "index.html";
    
    // Security check - prevent directory traversal
    if (path.indexOf("..") >= 0) {
        server.send(404, "text/plain", "Not Found");
        return;
    }
    
    // Map web paths to SD card paths
    String sdPath = WEB_FILES_PATH + path;
    
    File file = SD.open(sdPath);
    if (!file) {
        server.send(404, "text/plain", "File not found: " + sdPath);
        return;
    }
    
    // Determine content type
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".png")) contentType = "image/png";
    else if (path.endsWith(".jpg")) contentType = "image/jpeg";
    else if (path.endsWith(".ico")) contentType = "image/x-icon";
    
    // Stream file content
    server.streamFile(file, contentType);
    file.close();
}

// Volume control handlers
void handleVolumeUp() {
    int newVolume = increaseVolume();
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"volume\":" + String(newVolume) + "}");
}

void handleVolumeDown() {
    int newVolume = decreaseVolume();
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"volume\":" + String(newVolume) + "}");
}

void handleSetVolume() {
    if (server.hasArg("level")) {
        int newVolume = server.arg("level").toInt();
        if (setVolume(newVolume)) {
            server.send(200, "application/json", 
                "{\"status\":\"success\",\"volume\":" + String(newVolume) + "}");
        } else {
            server.send(400, "application/json", 
                "{\"status\":\"error\",\"message\":\"Volume must be between " + String(MIN_VOLUME) + " and " + String(MAX_VOLUME) + "\"}");
        }
    } else {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing volume level parameter\"}");
    }
}

void handleVolumeTest() {
    Serial.println("Volume test requested via web interface");
    testVolumeControl();
    server.send(200, "text/plain", "Volume diagnostic test completed. Check Serial Monitor for results.");
}

// Playback control handlers
void handleRandomPlay() {
    Serial.println("Random play requested via web interface");
    
    String musicFolder = "/music";
    if (server.hasArg("folder")) {
        musicFolder = server.arg("folder");
    }
    
    playRandomFile(musicFolder);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Random playback started from folder: " + musicFolder + "\"}");
}

void handleStop() {
    Serial.println("Stop playback requested via web interface");
    
    audio.stopSong();
    stopPlayback();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Playback stopped\"}");
}

void handlePause() {
    Serial.println("Pause playback requested via web interface");
    
    if (audio.isRunning()) {
        audio.pauseResume();
        server.send(200, "application/json", 
            "{\"status\":\"success\",\"message\":\"Playback paused\"}");
    } else {
        server.send(200, "application/json", 
            "{\"status\":\"info\",\"message\":\"No active playback to pause\"}");
    }
}

void handleResume() {
    Serial.println("Resume playback requested via web interface");
    
    audio.pauseResume();
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Playback resumed\"}");
}

// System handlers
void handleMemoryCheck() {
    Serial.println("Memory check requested via web interface");
    checkMemoryLimits();
    server.send(200, "text/plain", "Memory check completed. See Serial Monitor for detailed report.");
}

void handleWiFiReset() {
    Serial.println("WiFi reset requested via web interface");
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"WiFi settings will be reset. Device will restart.\"}");
    
    delay(1000);
    resetWiFiSettings();
}

void handleWiFiConfig() {
    Serial.println("WiFi config portal requested via web interface");
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"WiFi configuration portal starting. Connect to device AP to configure.\"}");
    
    delay(1000);
    startWiFiConfigPortal();
}

// Meme soundboard handlers
void handleMemeList() {
    String json = "{\"files\":[";
    const std::vector<String>& memes = getMemeFiles();
    for (size_t i = 0; i < memes.size(); ++i) {
        if (i > 0) json += ",";
        json += "\"" + memes[i] + "\"";
    }
    json += "]}";
    server.send(200, "application/json", json);
}

void handleMemePlay() {
    if (!server.hasArg("n")) {
        server.send(400, "text/plain", "Missing meme number");
        return;
    }
    
    int memeNum = server.arg("n").toInt();
    if (playMeme(memeNum)) {
        server.send(200, "text/plain", "Playing meme " + String(memeNum));
    } else {
        server.send(404, "text/plain", "Meme file not found");
    }
}

// Radio program handlers
void handleProgramShuffle() {
    Serial.println("Shuffle program requested via web interface");
    
    String musicFolder = "/music";
    if (server.hasArg("folder")) {
        musicFolder = server.arg("folder");
    }
    
    setProgramMode(SHUFFLE_PROGRAM, musicFolder);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Shuffle program started\",\"program\":\"SHUFFLE\",\"folder\":\"" + musicFolder + "\"}");
}

void handleProgramGenerative() {
    Serial.println("Generative program requested via web interface");
    
    String sequence = "";
    if (server.hasArg("sequence")) {
        sequence = server.arg("sequence");
    }
    
    setProgramMode(GENERATIVE_PROGRAM, sequence);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Generative program started\",\"program\":\"GENERATIVE\",\"sequence\":\"" + sequence + "\"}");
}

void handleProgramStream() {
    Serial.println("Stream program requested via web interface");
    
    String streamURL;
    if (server.hasArg("url")) {
        streamURL = server.arg("url");
    } else {
        // Use first available stream as default
        streamURL = "http://reggae.stream.laut.fm/reggae";
    }
    
    setProgramMode(STREAM_PROGRAM, streamURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Stream program started\",\"program\":\"STREAM\",\"url\":\"" + streamURL + "\"}");
}

void handleProgramNewStream() {
    Serial.println("NEW Stream program requested via web interface - FORCING NEW URL");
    
    audio.stopSong();
    delay(200);
    
    // Use the force new stream URL directly
    String forceNewURL = "https://reggae.stream.laut.fm/reggae";
    setProgramMode(STREAM_PROGRAM, forceNewURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"NEW Stream program started with forced URL\",\"program\":\"NEWSTREAM\",\"url\":\"" + forceNewURL + "\"}");
}

void handleProgramStatus() {
    server.send(200, "application/json", generateStatusJson());
}

// Program-specific handlers
void handleShuffleNext() {
    Serial.println("Shuffle next requested via web interface");
    
    if (getCurrentProgram() != SHUFFLE_PROGRAM) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Not in shuffle mode\"}");
        return;
    }
    
    playNextShuffleTrack();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Playing next shuffle track\"}");
}

void handleShuffleFolder() {
    Serial.println("Shuffle folder change requested via web interface");
    
    if (!server.hasArg("path")) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing folder path parameter\"}");
        return;
    }
    
    String folderPath = server.arg("path");
    buildShuffleQueue(folderPath);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Shuffle folder changed to: " + folderPath + "\"}");
}

void handleGenerativeSequence() {
    Serial.println("Generative sequence change requested via web interface");
    
    if (getCurrentProgram() != GENERATIVE_PROGRAM) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Not in generative mode\"}");
        return;
    }
    
    String sequence = "";
    if (server.hasArg("name")) {
        sequence = server.arg("name");
    }
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Generative sequence changed to: " + sequence + "\"}");
}

void handleGenerativeRegenerate() {
    Serial.println("Generative sequence regeneration requested via web interface");
    
    if (getCurrentProgram() != GENERATIVE_PROGRAM) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Not in generative mode\"}");
        return;
    }
    
    regenerateSequence();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"New generative sequence generated and will start shortly\"}");
}

void handleStreamConnect() {
    Serial.println("Stream connect requested via web interface");
    
    if (!server.hasArg("url")) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing stream URL parameter\"}");
        return;
    }
    
    String streamURL = server.arg("url");
    connectToStream(streamURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Connecting to stream: " + streamURL + "\"}");
}

void handleStreamReset() {
    Serial.println("Stream reset requested via web interface");
    
    clearStreamCache();
    // Use default stream URL directly
    String newURL = "http://reggae.stream.laut.fm/reggae";
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Stream cache cleared and reset to default URL\",\"url\":\"" + newURL + "\"}");
}
