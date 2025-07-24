/**
 * @file generative_manager.cpp
 * @brief Manages generative/ambient music playback using bell sounds.
 */

#include "generative_manager.h"
#include "../hardware/hardware_setup.h"
#include "../config/musicdata.h"
#include <SD.h>

// Generative state with sequence management
static GenerativeState generativeState = {
    .generativeActive = false,
    .lastNoteTime = 0,
    .nextNoteDelay = 2000
};

// Sequence management
static std::vector<String> currentSequence;
static size_t currentSequenceIndex = 0;

GenerativeState& getGenerativeState() {
    return generativeState;
}


/**
 * @brief Encapsulate note playback logic
 */
bool playNote(const String& soundFile) {
    // Serial.println("Playing generative note: " + soundFile);
    if (audio.connecttoFS(SD, soundFile.c_str())) {
        return true;
    } else {
        Serial.println("Failed to play: " + soundFile);
        return false;
    }
}

/**
 * @brief Encapsulate timing logic
 */
void setNextNoteDelay(bool success) {
    if (success) {
        // Random delay between 1-5 seconds for next note
        generativeState.nextNoteDelay = random(5000, 50000);
        Serial.println("Next note in " + String(generativeState.nextNoteDelay / 1000) + " seconds");
    } else {
        // Try again sooner if failed
        generativeState.nextNoteDelay = 500;
    }
    generativeState.lastNoteTime = millis();
}

/**
 * @brief Encapsulate error handling
 */
void handleNoFilesError() {
    Serial.println("No soundfont files found. Retrying in 5 seconds.");
    generativeState.lastNoteTime = millis();
    generativeState.nextNoteDelay = 5000;
}

/**
 * @brief Handles GENERATIVE program logic.
 */
void handleGenerativeProgram() {
    if (generativeState.generativeActive &&
        (millis() - generativeState.lastNoteTime >= generativeState.nextNoteDelay || !audio.isRunning())) {

        // Check if we need to generate a new sequence
        if (currentSequence.empty() || currentSequenceIndex >= currentSequence.size()) {
            // Retrieve soundfont files from SD card (using cached version)
            const std::vector<String>& soundfontFiles = getSoundfontFiles();
            if (soundfontFiles.empty()) {
                handleNoFilesError();
                return;
            }

            // Generate a harmonious sequence instead of random notes
            currentSequence.clear();
            currentSequenceIndex = 0;
            Serial.println("Generating harmonious sequence with " + String(soundfontFiles.size()) + " available files:");
            
            // Start with a random root note as the foundation of our harmony
            int rootNote = random(0, soundfontFiles.size());
            
            for (int i = 0; i < 200; ++i) {
                int noteIndex;
                
                // Create simple harmonic progression using musical intervals
                int progressionStep = i % 8; // Create an 8-note repeating pattern
                switch (progressionStep) {
                    case 0: case 4: 
                        noteIndex = rootNote; // Root note - foundation of the chord
                        break;                    
                    case 1: case 5: 
                        noteIndex = (rootNote + 4) % soundfontFiles.size(); // Major 3rd - adds sweetness
                        break;  
                    case 2: case 6: 
                        noteIndex = (rootNote + 7) % soundfontFiles.size(); // Perfect 5th - strong harmonic
                        break;  
                    case 3: case 7: 
                        noteIndex = (rootNote + 2) % soundfontFiles.size(); // Major 2nd - adds movement
                        break;  
                    default: 
                        noteIndex = rootNote; // Fallback to root
                        break;
                }
                
                currentSequence.push_back(soundfontFiles[noteIndex]);
                Serial.println("  [" + String(i + 1) + "] Harmonic index: " + String(noteIndex) + 
                              " (pattern step: " + String(progressionStep) + ")");
                
                // Change key every 32 notes for musical variety and progression
                if (i > 0 && i % 32 == 0) {
                    // Modulate to a nearby key (within 3 semitones up or down)
                    rootNote = (rootNote + random(-3, 4)) % soundfontFiles.size();
                    // Handle negative indices by wrapping around
                    if (rootNote < 0) rootNote += soundfontFiles.size();
                    Serial.println("  >>> Key change to root note: " + String(rootNote));
                }
            }
            Serial.println("Generated harmonious sequence of " + String(currentSequence.size()) + " notes");
        }

        // Play the current note in the sequence
        String selectedSound = currentSequence[currentSequenceIndex];
        Serial.println("Sequence [" + String(currentSequenceIndex + 1) + "/" + String(currentSequence.size()) + "]");
        bool success = playNote(selectedSound);
        
        // Move to next note in sequence
        currentSequenceIndex++;

        // Set the delay for the next note
        setNextNoteDelay(success);
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

/**
 * @brief Force regeneration of the current sequence.
 * @details Clears the current sequence and resets the index, forcing a new sequence to be generated on the next playback cycle.
 */
void regenerateSequence() {
    Serial.println("Forcing regeneration of generative sequence...");
    
    // Clear the current sequence to force regeneration
    currentSequence.clear();
    currentSequenceIndex = 0;
    
    // Reset timing to trigger immediate regeneration
    generativeState.lastNoteTime = 0;
    generativeState.nextNoteDelay = 500; // Short delay to start new sequence quickly
    
    Serial.println("Sequence cleared - new harmonious sequence will be generated on next cycle");
}
