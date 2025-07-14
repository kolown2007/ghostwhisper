/**
 * @file stream_manager.cpp
 * @brief Manages internet radio stream connections and playback.
 */

#include "stream_manager.h"
#include "hardware_setup.h"
#include "musicdata.h"

// Stream state
static StreamState streamState = {
    .currentStreamURL = "",
    .streamConnected = false,
    .reconnectAttempts = 0,
    .availableStreams = std::vector<String>()
};

StreamState& getStreamState() {
    return streamState;
}

bool isStreamConnected() {
    return streamState.streamConnected;
}

/**
 * @brief Connects to a stream URL.
 */
void connectToStream(const String& url) {
    Serial.println("=== STREAM CONNECTION ATTEMPT ===");
    Serial.println("Connecting to stream: " + url);
    
    // Stop any current playback first
    audio.stopSong();
    delay(500);
    
    streamState.currentStreamURL = url;
    streamState.streamConnected = false;
    streamState.reconnectAttempts = 0;
    
    Serial.println("Attempting to connect to host...");
    
    // Attempt to connect
    if (audio.connecttohost(url.c_str())) {
        streamState.streamConnected = true;
        Serial.println("✓ Stream connected successfully!");
        Serial.println("Audio volume: " + String(audio.getVolume()));
        delay(1000);
    } else {
        Serial.println("✗ Failed to connect to stream");
        streamState.reconnectAttempts++;
    }
    
    Serial.println("=== END STREAM CONNECTION ===");
}

/**
 * @brief Handles stream reconnection logic.
 */
void handleStreamReconnection() {
    if (streamState.currentStreamURL.length() > 0 && streamState.reconnectAttempts < 3) {
        Serial.println("Attempting stream reconnection (" + String(streamState.reconnectAttempts + 1) + "/3)");
        
        audio.stopSong();
        delay(2000);
        
        Serial.println("Clearing audio buffers before reconnection...");
        
        if (audio.connecttohost(streamState.currentStreamURL.c_str())) {
            streamState.streamConnected = true;
            Serial.println("Stream reconnected successfully");
            delay(1500);
        } else {
            streamState.reconnectAttempts++;
            Serial.println("Stream reconnection failed (attempt " + String(streamState.reconnectAttempts) + ")");
            
            if (streamState.reconnectAttempts >= 3) {
                Serial.println("All reconnection attempts failed, trying default stream...");
                String defaultURL = getDefaultStreamURL();
                if (defaultURL != streamState.currentStreamURL) {
                    streamState.currentStreamURL = defaultURL;
                    streamState.reconnectAttempts = 0;
                }
            }
        }
    }
}

/**
 * @brief Try the next available stream if current one has issues.
 */
void tryNextStream() {
    Serial.println("=== TRYING NEXT STREAM ===");
    
    if (streamState.availableStreams.size() == 0) {
        Serial.println("No alternative streams available");
        return;
    }
    
    // Get current stream index
    int currentIndex = -1;
    for (int i = 0; i < streamState.availableStreams.size(); i++) {
        if (streamState.availableStreams[i] == streamState.currentStreamURL) {
            currentIndex = i;
            break;
        }
    }
    
    // Move to next stream
    int nextIndex = (currentIndex + 1) % streamState.availableStreams.size();
    String nextURL = streamState.availableStreams[nextIndex];
    
    Serial.println("Switching from index " + String(currentIndex) + " to " + String(nextIndex));
    Serial.println("New stream: " + nextURL);
    
    connectToStream(nextURL);
    
    Serial.println("=== STREAM SWITCH COMPLETE ===");
}

/**
 * @brief Handles STREAM program logic.
 */
void handleStreamProgram() {
    static unsigned long lastErrorCheck = 0;
    static int consecutiveErrors = 0;
    
    // Monitor stream connection
    if (!streamState.streamConnected && streamState.reconnectAttempts < 3) {
        handleStreamReconnection();
        return;
    }
    
    // Check if stream is still active
    if (streamState.streamConnected && !audio.isRunning()) {
        Serial.println("Stream disconnected, attempting reconnection");
        streamState.streamConnected = false;
        streamState.reconnectAttempts++;
        return;
    }
    
    // Check for decode errors every 5 seconds
    if (millis() - lastErrorCheck > 5000) {
        lastErrorCheck = millis();
        
        // If we have persistent decode errors, try next stream
        if (consecutiveErrors > 10) {
            Serial.println("Too many consecutive decode errors, trying next stream...");
            tryNextStream();
            consecutiveErrors = 0;
        }
    }
}

/**
 * @brief Force clear any cached stream configuration.
 */
void clearStreamCache() {
    Serial.println("=== CLEARING STREAM CACHE ===");
    
    audio.stopSong();
    delay(500);
    
    streamState.currentStreamURL = "";
    streamState.streamConnected = false;
    streamState.reconnectAttempts = 0;
    
    // Force reload default URL
    String newURL = getDefaultStreamURL();
    Serial.println("Forced reload - new default URL: " + newURL);
    streamState.currentStreamURL = newURL;
    
    delay(100);
    
    Serial.println("=== STREAM CACHE CLEARED ===");
}
