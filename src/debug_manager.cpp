#include "debug_manager.h"

void logAudioStatus() {
    // Add audio status debugging
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 5000) { // Every 5 seconds
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
