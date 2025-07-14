/**
 * @file control.cpp
 * @brief Web server control interface for GhostWhisper system.
 * @details Provides HTTP endpoints for volume control, playback management, and system monitoring.
 */

#include "control.h"
#include "config.h"
#include "hardware_setup.h"
#include "playback_manager.h"
#include "musicdata.h"
#include <WebServer.h>
#include <WiFi.h>

// Web server instance
static WebServer server(WEB_SERVER_PORT);
static bool webServerActive = false;
static int currentVolume = DEFAULT_VOLUME; // Default volume - safer startup level

// Forward declarations
void handleRoot();
void handleVolumeUp();
void handleVolumeDown();
void handleSetVolume();
void handleStatus();
void handleNotFound();
void handleStaticFile();
void handleVolumeTest();
void handleRandomPlay();
void handleMemoryCheck();
void handleStop();
void handlePause();
void handleResume();
void handleProgramShuffle();
void handleProgramGenerative();
void handleProgramStream();
void handleProgramNewStream(); // NEW: Force new stream
void handleProgramStatus();
void handleShuffleNext();
void handleShuffleFolder();
void handleGenerativeSequence();
void handleGenerativeRegenerate();
void handleStreamConnect();
void handleStreamReset();
void checkMemoryLimits();
String generateMainPage();
String generateStatusJson();

// Helper function to sync volume with audio library
void syncVolumeWithAudio() {
    // Try to get current volume from audio library if method exists
    // For now, we'll ensure our local volume matches what we set
    audio.setVolume(currentVolume);
    Serial.println("Volume sync: Setting audio volume to " + String(currentVolume) + "%");
}

/**
 * @brief Initialize the web server control interface.
 */
void initializeWebControl() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Web server not started.");
        return;
    }

    // Synchronize with actual audio volume
    currentVolume = DEFAULT_VOLUME; // Set to quieter startup volume level
    audio.setVolume(currentVolume);
    Serial.println("Web control volume synchronized to: " + String(currentVolume));

    // Define HTTP routes
    server.on("/", handleRoot);
    server.on("/volume/up", handleVolumeUp);
    server.on("/volume/down", handleVolumeDown);
    server.on("/volume/set", handleSetVolume);
    server.on("/status", handleStatus);
    server.on("/test", handleVolumeTest);
    server.on("/random", handleRandomPlay);
    server.on("/memory", handleMemoryCheck);
    server.on("/stop", handleStop);
    server.on("/pause", handlePause);
    server.on("/resume", handleResume);
    
    // Radio program endpoints
    server.on("/program/shuffle", handleProgramShuffle);
    server.on("/program/generative", handleProgramGenerative);
    server.on("/program/stream", handleProgramStream);
    server.on("/program/newstream", handleProgramNewStream); // NEW: Force new stream
    server.on("/program/status", handleProgramStatus);
    
    // Program-specific endpoints
    server.on("/shuffle/next", handleShuffleNext);
    server.on("/shuffle/folder", handleShuffleFolder);
    server.on("/generative/sequence", handleGenerativeSequence);
    server.on("/generative/regenerate", handleGenerativeRegenerate);
    server.on("/stream/connect", handleStreamConnect);
    server.on("/stream/reset", handleStreamReset);
    
    // Handle static files from SD card
    server.on("/style.css", handleStaticFile);
    server.on("/app.js", handleStaticFile);
    server.on("/favicon.ico", handleStaticFile);
    
    server.onNotFound(handleNotFound);

    // Start the web server
    server.begin();
    webServerActive = true;
    
    Serial.println("Web server started successfully!");
    Serial.print("Access the control interface at: http://");
    Serial.print(WiFi.localIP());
    Serial.println(":" + String(WEB_SERVER_PORT));
    
    // Check memory usage and limits
    checkMemoryLimits();
    
    // REMOVED: Auto volume test that causes audio delays
    // User can manually run test via web interface /test endpoint
}

/**
 * @brief Handle incoming web server requests.
 */
void handleWebControl() {
    if (webServerActive) {
        server.handleClient();
    }
}

/**
 * @brief Check if the web server is running.
 */
bool isWebServerActive() {
    return webServerActive;
}

/**
 * @brief Get the current web server status.
 */
String getWebServerStatus() {
    if (webServerActive) {
        return "Web server active at " + WiFi.localIP().toString() + ":80";
    } else {
        return "Web server inactive";
    }
}

/**
 * @brief Stop the web server.
 */
void stopWebServer() {
    if (webServerActive) {
        server.stop();
        webServerActive = false;
        Serial.println("Web server stopped.");
    }
}

/**
 * @brief Restart the web server.
 */
void restartWebServer() {
    stopWebServer();
    delay(100);
    initializeWebControl();
}

// HTTP Route Handlers

/**
 * @brief Handle root page request.
 */
void handleRoot() {
    server.send(200, "text/html", generateMainPage());
}

/**
 * @brief Handle volume increase request.
 */
void handleVolumeUp() {
    if (currentVolume < MAX_VOLUME) {
        currentVolume += VOLUME_STEP;
        if (currentVolume > MAX_VOLUME) currentVolume = MAX_VOLUME;
        
        // Test multiple approaches to volume control
        Serial.println("Volume UP - Attempting to set to: " + String(currentVolume) + "%");
        
        // Get current playing status
        bool wasPlaying = audio.isRunning();
        String currentURL = ""; // We'll store this if needed
        
        // Method 1: Direct volume change
        audio.setVolume(currentVolume);
        Serial.println("Method 1: Direct setVolume() called");
        
        // Method 2: If direct doesn't work, try stopping and restarting
        if (wasPlaying) {
            Serial.println("Method 2: Audio was playing, testing restart approach...");
            // Note: This is more aggressive and might interrupt playback
            // Uncomment if direct volume control doesn't work:
            // audio.stopSong();
            // delay(100);
            // audio.setVolume(currentVolume);
            // audio.connecttohost("your_stream_url_here"); // Would need to store current URL
        }
        
        // Method 3: Check if volume actually changed
        delay(100); // Give audio library time to process
        Serial.println("Volume change command completed. Testing audio responsiveness...");
        
    } else {
        Serial.println("Volume UP - Already at maximum (" + String(MAX_VOLUME) + "%)");
    }
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"volume\":" + String(currentVolume) + "}");
}

/**
 * @brief Handle volume decrease request.
 */
void handleVolumeDown() {
    if (currentVolume > MIN_VOLUME) {
        currentVolume -= VOLUME_STEP;
        if (currentVolume < MIN_VOLUME) currentVolume = MIN_VOLUME;
        
        // Test volume control with enhanced debugging
        Serial.println("Volume DOWN - Attempting to set to: " + String(currentVolume) + "%");
        
        // Direct volume change
        audio.setVolume(currentVolume);
        Serial.println("setVolume() called with value: " + String(currentVolume));
        
        // Add enhanced debugging
        delay(100);
        Serial.println("Volume change processing completed");
        
    } else {
        Serial.println("Volume DOWN - Already at minimum (" + String(MIN_VOLUME) + "%)");
    }
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"volume\":" + String(currentVolume) + "}");
}

/**
 * @brief Handle set volume request.
 */
void handleSetVolume() {
    if (server.hasArg("level")) {
        int newVolume = server.arg("level").toInt();
        if (newVolume >= MIN_VOLUME && newVolume <= MAX_VOLUME) {
            Serial.println("Volume SLIDER - Changing from " + String(currentVolume) + "% to " + String(newVolume) + "%");
            currentVolume = newVolume;
            
            // Enhanced volume setting with detailed logging
            Serial.println("Calling audio.setVolume(" + String(currentVolume) + ")...");
            audio.setVolume(currentVolume);
            Serial.println("setVolume() call completed");
            
            // Add delay to ensure volume change is processed
            delay(100);
            Serial.println("Volume setting operation completed");
            
            server.send(200, "application/json", 
                "{\"status\":\"success\",\"volume\":" + String(currentVolume) + "}");
        } else {
            Serial.println("Volume SLIDER - Invalid level: " + String(newVolume));
            server.send(400, "application/json", 
                "{\"status\":\"error\",\"message\":\"Volume must be between " + String(MIN_VOLUME) + " and " + String(MAX_VOLUME) + "\"}");
        }
    } else {
        Serial.println("Volume SLIDER - Missing level parameter");
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing volume level parameter\"}");
    }
}

/**
 * @brief Handle status request.
 */
void handleStatus() {
    server.send(200, "application/json", generateStatusJson());
}

/**
 * @brief Handle 404 not found requests with better error information.
 */
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

/**
 * @brief Handle volume test request.
 */
void handleVolumeTest() {
    Serial.println("Volume test requested via web interface");
    testVolumeControl();
    server.send(200, "text/plain", "Volume diagnostic test completed. Check Serial Monitor for results.");
}

/**
 * @brief Handle random play request.
 */
void handleRandomPlay() {
    Serial.println("Random play requested via web interface");
    
    // Get the music folder parameter (default to "/music")
    String musicFolder = "/music";
    if (server.hasArg("folder")) {
        musicFolder = server.arg("folder");
    }
    
    // Call the random play function
    playRandomFile(musicFolder);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Random playback started from folder: " + musicFolder + "\"}");
}

/**
 * @brief Handle memory check request.
 */
void handleMemoryCheck() {
    Serial.println("Memory check requested via web interface");
    checkMemoryLimits();
    server.send(200, "text/plain", "Memory check completed. See Serial Monitor for detailed report.");
}

/**
 * @brief Handle stop playback request.
 */
void handleStop() {
    Serial.println("Stop playback requested via web interface");
    
    // Stop the audio
    audio.stopSong();
    
    // Reset playback manager state
    stopPlayback();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Playback stopped\"}");
}

/**
 * @brief Handle pause playback request.
 */
void handlePause() {
    Serial.println("Pause playback requested via web interface");
    
    // Check if audio is currently running
    if (audio.isRunning()) {
        audio.pauseResume();
        server.send(200, "application/json", 
            "{\"status\":\"success\",\"message\":\"Playback paused\"}");
    } else {
        server.send(200, "application/json", 
            "{\"status\":\"info\",\"message\":\"No active playback to pause\"}");
    }
}

/**
 * @brief Handle resume playback request.
 */
void handleResume() {
    Serial.println("Resume playback requested via web interface");
    
    // Resume audio playback
    audio.pauseResume();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Playback resumed\"}");
}

/**
 * @brief Serve static files from SD card (CSS, JS, images, etc.)
 */
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

// Radio Program Handlers

/**
 * @brief Handle shuffle program request.
 */
void handleProgramShuffle() {
    Serial.println("Shuffle program requested via web interface");
    
    // Get the music folder parameter (default to "/music")
    String musicFolder = "/music";
    if (server.hasArg("folder")) {
        musicFolder = server.arg("folder");
    }
    
    // Set shuffle program mode
    setProgramMode(SHUFFLE_PROGRAM, musicFolder);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Shuffle program started\",\"program\":\"SHUFFLE\",\"folder\":\"" + musicFolder + "\"}");
}

/**
 * @brief Handle generative program request.
 */
void handleProgramGenerative() {
    Serial.println("Generative program requested via web interface");
    
    // Get optional sequence parameter
    String sequence = "";
    if (server.hasArg("sequence")) {
        sequence = server.arg("sequence");
    }
    
    // Set generative program mode
    setProgramMode(GENERATIVE_PROGRAM, sequence);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Generative program started\",\"program\":\"GENERATIVE\",\"sequence\":\"" + sequence + "\"}");
}

/**
 * @brief Handle stream program request.
 */
void handleProgramStream() {
    Serial.println("Stream program requested via web interface");
    
    String streamURL;
    
    // Get the stream URL parameter (optional)
    if (server.hasArg("url")) {
        streamURL = server.arg("url");
        Serial.println("Using provided stream URL: " + streamURL);
    } else {
        // Use default stream URL from musicdata.h
        streamURL = getDefaultStreamURL();
        Serial.println("Using default stream URL from musicdata.h: " + streamURL);
    }
    
    // Set stream program mode
    setProgramMode(STREAM_PROGRAM, streamURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Stream program started\",\"program\":\"STREAM\",\"url\":\"" + streamURL + "\"}");
}

/**
 * @brief Handle NEW stream program request with forced new URL.
 */
void handleProgramNewStream() {
    Serial.println("=== NEW STREAM PROGRAM REQUESTED ===");
    Serial.println("NEW Stream program requested via web interface - FORCING NEW URL");
    
    // Stop any current playback first
    audio.stopSong();
    delay(200);
    
    // Force use the NEW stream URL - bypass any cached state
    String forceNewURL = getForceNewStreamURL();
    Serial.println("=== FORCING NEW STREAM URL: " + forceNewURL + " ===");
    
    // Set stream program mode with the forced new URL
    setProgramMode(STREAM_PROGRAM, forceNewURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"NEW Stream program started with forced URL\",\"program\":\"NEWSTREAM\",\"url\":\"" + forceNewURL + "\"}");
    
    Serial.println("=== NEW STREAM SETUP COMPLETE ===");
}

/**
 * @brief Handle program status request.
 */
void handleProgramStatus() {
    Serial.println("Program status requested via web interface");
    
    ProgramState& state = getProgramState();
    String programName;
    
    switch (state.currentProgram) {
        case SHUFFLE_PROGRAM:
            programName = "SHUFFLE";
            break;
        case GENERATIVE_PROGRAM:
            programName = "GENERATIVE";
            break;
        case STREAM_PROGRAM:
            programName = "STREAM";
            break;
    }
    
    String json = "{";
    json += "\"currentProgram\":\"" + programName + "\",";
    json += "\"programActive\":" + String(state.programActive ? "true" : "false") + ",";
    
    // Program-specific status
    if (state.currentProgram == SHUFFLE_PROGRAM) {
        json += "\"shuffleQueue\":" + String(state.shuffleQueue.size()) + ",";
        json += "\"musicFolder\":\"" + state.musicFolder + "\",";
        json += "\"autoAdvance\":" + String(state.shuffleAutoAdvance ? "true" : "false") + ",";
    } else if (state.currentProgram == GENERATIVE_PROGRAM) {
        json += "\"generativeActive\":" + String(state.generativeActive ? "true" : "false") + ",";
        json += "\"waitingForTransition\":" + String(state.waitingForTransition ? "true" : "false") + ",";
    } else if (state.currentProgram == STREAM_PROGRAM) {
        json += "\"streamURL\":\"" + state.currentStreamURL + "\",";
        json += "\"streamConnected\":" + String(state.streamConnected ? "true" : "false") + ",";
        json += "\"reconnectAttempts\":" + String(state.reconnectAttempts) + ",";
    }
    
    json += "\"audioRunning\":" + String(audio.isRunning() ? "true" : "false");
    json += "}";
    
    server.send(200, "application/json", json);
}

/**
 * @brief Handle shuffle next track request.
 */
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

/**
 * @brief Handle shuffle folder change request.
 */
void handleShuffleFolder() {
    Serial.println("Shuffle folder change requested via web interface");
    
    if (!server.hasArg("path")) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing folder path parameter\"}");
        return;
    }
    
    String folderPath = server.arg("path");
    
    // Rebuild shuffle queue with new folder
    buildShuffleQueue(folderPath);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Shuffle folder changed to: " + folderPath + "\"}");
}

/**
 * @brief Handle generative sequence change request.
 */
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
    
    // Note: You might want to add sequence change logic here
    // For now, just acknowledge the request
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Generative sequence changed to: " + sequence + "\"}");
}

/**
 * @brief Handle stream connect request.
 */
void handleStreamConnect() {
    Serial.println("Stream connect requested via web interface");
    
    if (!server.hasArg("url")) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Missing stream URL parameter\"}");
        return;
    }
    
    String streamURL = server.arg("url");
    
    // Connect to new stream
    connectToStream(streamURL);
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Connecting to stream: " + streamURL + "\"}");
}

/**
 * @brief Handle stream reset request - clears cache and forces reload of default URL.
 */
void handleStreamReset() {
    Serial.println("Stream reset requested via web interface");
    
    // Clear stream cache and force reload
    clearStreamCache();
    
    // Get the new default URL
    String newURL = getDefaultStreamURL();
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"Stream cache cleared and reset to default URL\",\"url\":\"" + newURL + "\"}");
}

/**
 * @brief Handle generative sequence regeneration request.
 */
void handleGenerativeRegenerate() {
    Serial.println("Generative sequence regeneration requested via web interface");
    
    if (getCurrentProgram() != GENERATIVE_PROGRAM) {
        server.send(400, "application/json", 
            "{\"status\":\"error\",\"message\":\"Not in generative mode\"}");
        return;
    }
    
    server.send(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"New generative sequence generated\"}");
}

/**
 * @brief Load HTML file from SD card and process template variables.
 */
String loadHTMLFromSD(const char* filename) {
    String fullPath = String(WEB_FILES_PATH) + filename;
    File htmlFile = SD.open(fullPath);
    if (!htmlFile) {
        Serial.println("Failed to open HTML file: " + fullPath);
        return "<html><body><h1>Error: Web files not found on SD card</h1><p>Please copy web files to SD card " + String(WEB_FILES_PATH) + " folder</p></body></html>";
    }
    
    String html = "";
    while (htmlFile.available()) {
        html += char(htmlFile.read());
    }
    htmlFile.close();
    
    // Process template variables
    html.replace("%VOLUME%", String(currentVolume));
    html.replace("%WIFI_IP%", WiFi.localIP().toString());
    html.replace("%UPTIME%", String(millis() / 1000));
    html.replace("%FREE_HEAP%", String(ESP.getFreeHeap()));
    
    return html;
}

/**
 * @brief Generate the main control page HTML.
 */
String generateMainPage() {
    return loadHTMLFromSD("/index.html");
}

/**
 * @brief Generate system status JSON.
 */
String generateStatusJson() {
    ProgramState& state = getProgramState();
    String programName;
    
    switch (state.currentProgram) {
        case SHUFFLE_PROGRAM:
            programName = "SHUFFLE";
            break;
        case GENERATIVE_PROGRAM:
            programName = "GENERATIVE";
            break;
        case STREAM_PROGRAM:
            programName = "STREAM";
            break;
    }
    
    String json = "{";
    json += "\"volume\":" + String(currentVolume) + ",";
    json += "\"wifi\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"totalHeap\":" + String(ESP.getHeapSize()) + ",";
    json += "\"flashSize\":" + String(ESP.getFlashChipSize()) + ",";
    json += "\"freeSketchSpace\":" + String(ESP.getFreeSketchSpace()) + ",";
    json += "\"audioRunning\":" + String(audio.isRunning() ? "true" : "false") + ",";
    json += "\"playbackActive\":" + String(isPlaybackActive() ? "true" : "false") + ",";
    json += "\"currentProgram\":\"" + programName + "\",";
    json += "\"programActive\":" + String(state.programActive ? "true" : "false");
    json += "}";
    
    return json;
}

/**
 * @brief Check if the program will fit on ESP32 and log memory info.
 */
void checkMemoryLimits() {
    Serial.println("=== MEMORY USAGE REPORT ===");
    
    // Flash memory (program storage)
    size_t flashSize = ESP.getFlashChipSize();
    size_t sketchSize = ESP.getSketchSize();
    size_t freeSketchSpace = ESP.getFreeSketchSpace();
    
    float flashUsedPercent = ((float)sketchSize / flashSize) * 100;
    
    Serial.printf("Flash Memory:\n");
    Serial.printf("  Total Flash: %d bytes (%.1f MB)\n", flashSize, flashSize / 1048576.0);
    Serial.printf("  Sketch Size: %d bytes (%.1f KB)\n", sketchSize, sketchSize / 1024.0);
    Serial.printf("  Free Space:  %d bytes (%.1f KB)\n", freeSketchSpace, freeSketchSpace / 1024.0);
    Serial.printf("  Used: %.1f%%\n", flashUsedPercent);
    
    // RAM memory (runtime)
    size_t totalHeap = ESP.getHeapSize();
    size_t freeHeap = ESP.getFreeHeap();
    size_t usedHeap = totalHeap - freeHeap;
    
    float ramUsedPercent = ((float)usedHeap / totalHeap) * 100;
    
    Serial.printf("RAM Memory:\n");
    Serial.printf("  Total RAM: %d bytes (%.1f KB)\n", totalHeap, totalHeap / 1024.0);
    Serial.printf("  Used RAM:  %d bytes (%.1f KB)\n", usedHeap, usedHeap / 1024.0);
    Serial.printf("  Free RAM:  %d bytes (%.1f KB)\n", freeHeap, freeHeap / 1024.0);
    Serial.printf("  Used: %.1f%%\n", ramUsedPercent);
    
    // Warnings
    if (flashUsedPercent > 80) {
        Serial.println("⚠️  WARNING: Flash usage > 80% - Consider removing features!");
    }
    if (flashUsedPercent > 90) {
        Serial.println("🚨 CRITICAL: Flash usage > 90% - May not fit on ESP32!");
    }
    if (ramUsedPercent > 70) {
        Serial.println("⚠️  WARNING: RAM usage > 70% - System may become unstable!");
    }
    if (ramUsedPercent > 85) {
        Serial.println("🚨 CRITICAL: RAM usage > 85% - System likely to crash!");
    }
    
    Serial.println("===========================");
}

/**
 * @brief Test audio library volume functionality
 */
void testVolumeControl() {
    Serial.println("=== AUDIO VOLUME DIAGNOSTIC TEST ===");
    
    // Test if audio object exists and is responsive
    Serial.println("Testing audio library responsiveness...");
    
    // Check current state
    bool audioIsRunning = audio.isRunning();
    Serial.println("Audio status: " + String(audioIsRunning ? "RUNNING" : "STOPPED"));
    
    if (!audioIsRunning) {
        Serial.println("WARNING: Audio is not running. Volume control effectiveness may be limited.");
        Serial.println("For full testing, start audio playback first.");
    }
    
    // Store current volume
    int originalVolume = currentVolume;
    Serial.println("Original volume setting: " + String(originalVolume) + "%");
    
    // Test extreme volume changes to see if any change is detectable
    Serial.println("Testing extreme volume changes:");
    
    Serial.println("Setting volume to 0% (mute)...");
    audio.setVolume(0);
    delay(100); // Reduced delay - 100ms
    
    Serial.println("Setting volume to 100% (maximum)...");
    audio.setVolume(100);
    delay(100); // Reduced delay - 100ms
    
    Serial.println("Setting volume to 50% (moderate)...");
    audio.setVolume(50);
    delay(100); // Reduced delay - 100ms
    
    // Restore original volume
    Serial.println("Restoring original volume: " + String(originalVolume) + "%");
    audio.setVolume(originalVolume);
    currentVolume = originalVolume; // Keep our tracking in sync
    
    Serial.println("=== DIAGNOSIS COMPLETE ===");
    Serial.println("If you heard volume changes during this test, software volume control is working.");
    Serial.println("If no volume changes were heard:");
    Serial.println("1. Check I2S DAC hardware connections");
    Serial.println("2. Verify I2S DAC supports software volume control");
    Serial.println("3. Check if audio stream needs to be restarted for volume changes");
    Serial.println("4. Consider using external volume control hardware");
    Serial.println("============================");
}
