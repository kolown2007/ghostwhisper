/**
 * @file web_utils.cpp
 * @brief Web utility functions implementation
 */

#include "web_utils.h"
#include "../config/config.h"
#include "../hardware/volume_control.h"
#include "../managers/radio_manager.h"
#include "../managers/connection_manager.h"
#include "../hardware/hardware_setup.h"
#include <WiFi.h>
#include <SD.h>
#include <Audio.h>

// External references
extern Audio audio;

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
    html.replace("%VOLUME%", String(getCurrentVolume()));
    html.replace("%WIFI_IP%", WiFi.localIP().toString());
    html.replace("%UPTIME%", String(millis() / 1000));
    html.replace("%FREE_HEAP%", String(ESP.getFreeHeap()));
    
    return html;
}

String generateMainPage() {
    return loadHTMLFromSD("/index.html");
}

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
    json += "\"volume\":" + String(getCurrentVolume()) + ",";
    json += "\"wifi\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"totalHeap\":" + String(ESP.getHeapSize()) + ",";
    json += "\"flashSize\":" + String(ESP.getFlashChipSize()) + ",";
    json += "\"freeSketchSpace\":" + String(ESP.getFreeSketchSpace()) + ",";
    json += "\"playbackActive\":" + String(isPlaybackActive() ? "true" : "false") + ",";
    json += "\"currentProgram\":\"" + programName + "\",";
    json += "\"programActive\":" + String(state.programActive ? "true" : "false") + ",";
    json += "\"connectionMode\":\"" + String(getConnectionMode() == ONLINE ? "ONLINE" : "OFFLINE") + "\"";
    json += "}";
    
    return json;
}

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
