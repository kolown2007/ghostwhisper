#include "debug_manager.h"
#include "../config/config.h"

void logAudioStatus() {
    // Add audio status debugging - reduced frequency
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        Serial.println("Audio status - Current time: " + String(millis()));
        lastDebugTime = millis();
    }
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
