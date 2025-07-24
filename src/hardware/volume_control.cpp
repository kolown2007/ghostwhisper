/**
 * @file volume_control.cpp
 * @brief Volume control functionality implementation
 */

#include "volume_control.h"
#include "../config/config.h"
#include "hardware_setup.h"

static int currentVolume = DEFAULT_VOLUME;
static bool isPaused = false;

int getCurrentVolume() {
    return currentVolume;
}

bool setVolume(int volume) {
    if (volume < MIN_VOLUME || volume > MAX_VOLUME) {
        return false;
    }
    
    currentVolume = volume;
    audio.setVolume(currentVolume);
    Serial.println("Volume set to: " + String(currentVolume) + "%");
    return true;
}

int increaseVolume() {
    if (currentVolume < MAX_VOLUME) {
        currentVolume += VOLUME_STEP;
        if (currentVolume > MAX_VOLUME) currentVolume = MAX_VOLUME;
        
        Serial.println("Volume UP - Setting to: " + String(currentVolume) + "%");
        audio.setVolume(currentVolume);
        delay(100);
    } else {
        Serial.println("Volume UP - Already at maximum (" + String(MAX_VOLUME) + "%)");
    }
    return currentVolume;
}

int decreaseVolume() {
    if (currentVolume > MIN_VOLUME) {
        currentVolume -= VOLUME_STEP;
        if (currentVolume < MIN_VOLUME) currentVolume = MIN_VOLUME;
        
        Serial.println("Volume DOWN - Setting to: " + String(currentVolume) + "%");
        audio.setVolume(currentVolume);
        delay(100);
    } else {
        Serial.println("Volume DOWN - Already at minimum (" + String(MIN_VOLUME) + "%)");
    }
    return currentVolume;
}

void syncVolumeWithAudio() {
    audio.setVolume(currentVolume);
    Serial.println("Volume sync: Setting audio volume to " + String(currentVolume) + "%");
}

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
    delay(100);
    
    Serial.println("Setting volume to 100% (maximum)...");
    audio.setVolume(100);
    delay(100);
    
    Serial.println("Setting volume to 50% (moderate)...");
    audio.setVolume(50);
    delay(100);
    
    // Restore original volume
    Serial.println("Restoring original volume: " + String(originalVolume) + "%");
    audio.setVolume(originalVolume);
    currentVolume = originalVolume;
    
    Serial.println("=== DIAGNOSIS COMPLETE ===");
    Serial.println("If you heard volume changes during this test, software volume control is working.");
    Serial.println("If no volume changes were heard:");
    Serial.println("1. Check I2S DAC hardware connections");
    Serial.println("2. Verify I2S DAC supports software volume control");
    Serial.println("3. Check if audio stream needs to be restarted for volume changes");
    Serial.println("4. Consider using external volume control hardware");
    Serial.println("============================");
}

void initializeVolumeControl() {
    currentVolume = DEFAULT_VOLUME;
    audio.setVolume(currentVolume);
    Serial.println("Volume control initialized to: " + String(currentVolume) + "%");
}
