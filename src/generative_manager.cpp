/**
 * @file generative_manager.cpp
 * @brief Manages generative/ambient music playback using bell sounds.
 */

#include "generative_manager.h"
#include "hardware_setup.h"
#include "musicdata.h"
#include <SD.h>

// Generative state
static GenerativeState generativeState = {
    .generativeActive = false,
    .lastNoteTime = 0,
    .nextNoteDelay = 2000
};

GenerativeState& getGenerativeState() {
    return generativeState;
}

/**
 * @brief Handles GENERATIVE program logic.
 */
void handleGenerativeProgram() {
    // Check if it's time to play the next note
    if (generativeState.generativeActive && 
        (millis() - generativeState.lastNoteTime >= generativeState.nextNoteDelay || !audio.isRunning())) {
        
        // Get soundfont files from a random subfolder
        const std::vector<String>& soundfontFiles = getSoundfontFiles();
        
        if (soundfontFiles.size() > 0) {
            // Pick a random soundfont file
            int randomIndex = random(0, soundfontFiles.size());
            String selectedSound = soundfontFiles[randomIndex];
            
            Serial.println("Playing generative note: " + selectedSound);
            
            // Play the sound from SD card
            if (audio.connecttoFS(SD, selectedSound.c_str())) {
                generativeState.lastNoteTime = millis();
                
                // Random delay between 1-5 seconds for next note
                generativeState.nextNoteDelay = random(1000, 5000);
                
                Serial.println("Next note in " + String(generativeState.nextNoteDelay/1000) + " seconds");
            } else {
                Serial.println("Failed to play: " + selectedSound);
                // Try again sooner if failed
                generativeState.nextNoteDelay = 500;
            }
        } else {
            Serial.println("No soundfont files found in the selected folder");
            // Check again in 5 seconds
            generativeState.lastNoteTime = millis();
            generativeState.nextNoteDelay = 5000;
        }
    }
}

/**
 * @brief Convenience function to start generative playback.
 */
void playSequence() {
    generativeState.generativeActive = true;
    generativeState.lastNoteTime = 0;
    generativeState.nextNoteDelay = 2000;
    
    // Load soundfont files to make sure they're available
    const std::vector<String>& soundfontFiles = getSoundfontFiles();
    Serial.println("Generative program activated with " + String(soundfontFiles.size()) + " soundfont files");
}
