#include "debug_manager.h"
#include "config.h"

void logAudioStatus() {
    // Add audio status debugging - reduced frequency
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        Serial.println("Audio status - Current time: " + String(millis()));
        lastDebugTime = millis();
    }
}

void setupAudioCallbacks() {
    // Audio callbacks are automatically called by the Audio library
    // No explicit setup needed, just defining the functions
}

// Audio event callbacks for debugging
void audio_info(const char *info) {
    Serial.print("Audio info: "); Serial.println(info);
}

void audio_id3data(const char *info) {
    Serial.print("ID3 data: "); Serial.println(info);  
}

void audio_eof_mp3(const char *info) {
    Serial.print("End of file: "); Serial.println(info);
}

void audio_showstation(const char *info) {
    Serial.print("Station: "); Serial.println(info);
}

void audio_showstreamtitle(const char *info) {
    Serial.print("Stream title: "); Serial.println(info);
    // Detect 404 errors and skip problematic notes
    if (String(info).indexOf("404") != -1) {
        Serial.println("WARNING: 404 Error detected - this URL doesn't exist!");
    }
}

void audio_bitrate(const char *info) {
    Serial.print("Bitrate: "); Serial.println(info);
}

void audio_commercial(const char *info) {
    Serial.print("Commercial: "); Serial.println(info);
}

void audio_icyurl(const char *info) {
    Serial.print("ICY URL: "); Serial.println(info);
}

void audio_lasthost(const char *info) {
    Serial.print("Last host: "); Serial.println(info);
}

/**
 * @brief Monitor system memory and log warnings if low.
 */
void monitorSystemHealth() {
    static unsigned long lastHealthCheck = 0;
    static size_t minHeapSeen = SIZE_MAX;
    
    if (millis() - lastHealthCheck > HEALTH_CHECK_INTERVAL_MS) {
        size_t freeHeap = ESP.getFreeHeap();
        
        if (freeHeap < minHeapSeen) {
            minHeapSeen = freeHeap;
        }
        
        Serial.printf("Health Check - Free heap: %d bytes (min seen: %d)\n", freeHeap, minHeapSeen);
        
        // Warn if memory is getting low
        if (freeHeap < LOW_MEMORY_THRESHOLD) {
            Serial.println("WARNING: Low memory detected!");
        }
        
        lastHealthCheck = millis();
    }
}
