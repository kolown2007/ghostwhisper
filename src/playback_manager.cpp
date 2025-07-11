/**
 * @file playback_manager.cpp
 * @brief Manages playback modes and logic for sequence-based and direct file playback.
 */

#include "playback_manager.h"
#include "hardware_setup.h"
#include "sequence_manager.h"
#include <SD.h>
#include <vector>

// Playback state variables
static PlaybackMode currentMode = SEQUENCE;
static String directPlayFile = "";
static bool playbackActive = false;

// Sequence mode variables
static unsigned long lastNotePlayTime = 0;
static unsigned long noteTransitionTime = 0;
static bool waitingForTransition = false;
static String nextNoteUrl = "";

// Direct mode variables
static bool directPlayStarted = false;

void handleSequencePlayback(); // Forward declaration
void handleDirectPlayback();   // Forward declaration

/**
 * @brief Sets the playback mode and initializes playback state.
 * @param mode The playback mode (SEQUENCE or DIRECT).
 * @param file The file to play in DIRECT mode (can be SD card path or web URL).
 */
void setPlaybackMode(PlaybackMode mode, const String& file) {
    currentMode = mode;
    directPlayFile = file;
    playbackActive = false;
    directPlayStarted = false;
    waitingForTransition = false;
    
    Serial.print("Playback mode set to: ");
    Serial.println(mode == SEQUENCE ? "SEQUENCE" : "DIRECT");
    if (mode == DIRECT && file.length() > 0) {
        Serial.println("Direct file: " + file);
    }
}

/**
 * @brief Handles playback logic based on the current mode.
 */
void handlePlayback() {
    if (currentMode == SEQUENCE) {
        handleSequencePlayback();
    } else if (currentMode == DIRECT) {
        handleDirectPlayback();
    }
}

/**
 * @brief Handles sequence-based playback logic.
 */
void handleSequencePlayback() {
    // This is the existing sequence logic
    handleTransitions();
}

/**
 * @brief Handles direct file playback logic.
 */
void handleDirectPlayback() {
    if (!directPlayStarted && directPlayFile.length() > 0) {
        Serial.println("Starting direct playback: " + directPlayFile);
        
        // Play the file directly
        if (directPlayFile.startsWith("/")) {
            // SD card file
            Serial.println("Playing from SD card: " + directPlayFile);
            audio.connecttoFS(SD, directPlayFile.c_str());
        } else {
            // Web URL
            Serial.println("Streaming from web: " + directPlayFile);
            audio.connecttohost(directPlayFile.c_str());
        }
        
        directPlayStarted = true;
        playbackActive = true;
    }
    
    // Monitor if direct playback is still active
    // Note: You might want to add audio.isRunning() check here if available
}

/**
 * @brief Checks if playback is currently active.
 * @return True if playback is active, false otherwise.
 */
bool isPlaybackActive() {
    return playbackActive;
}

/**
 * @brief Stops the current playback and resets playback state.
 */
void stopPlayback() {
    Serial.println("Stopping playback");
    // Note: Audio library may not have stop function, might need audio.stop() if available
    playbackActive = false;
    directPlayStarted = false;
    waitingForTransition = false;
}

/**
 * @brief Handles transitions between notes in sequence playback.
 */
void handleTransitions() {
    unsigned long now = millis();
    
    // Handle transition delays with proper gap before playing
    if (waitingForTransition) {
        if (now - noteTransitionTime >= 200) { // 200ms gap before playing next note
            waitingForTransition = false;
            
            // Now play the note after the gap
            Serial.println("Gap complete, playing: " + nextNoteUrl);
            
            // Check if it's an SD card file or web URL
            if (nextNoteUrl.startsWith("/")) {
                // SD card file - use connecttoFS
                Serial.println("Playing from SD card: " + nextNoteUrl);
                audio.connecttoFS(SD, nextNoteUrl.c_str());
            } else {
                // Web URL - use connecttohost
                Serial.println("Streaming from web: " + nextNoteUrl);
                audio.connecttohost(nextNoteUrl.c_str());
            }
            
            lastNotePlayTime = now;
            advanceToNextNote();
            
            Serial.println("Started file " + String(getCurrentNoteIndex()) + "/" + String(getSequenceSize()));
        }
    }
}

/**
 * @brief Starts a transition gap before playing the next note.
 * @param url The URL or file path of the next note.
 */
void playNextNote(const String& url) {
    // Start transition: store URL and begin gap period
    Serial.println("Starting transition gap for next note");
    nextNoteUrl = url;
    waitingForTransition = true;
    noteTransitionTime = millis();
}

/**
 * @brief Initiates a transition to the next note.
 * @param url The URL or file path of the next note.
 */
void startTransition(const String& url) {
    playNextNote(url);
}

/**
 * @brief Checks if the system is waiting for a transition gap.
 * @return True if waiting for a transition, false otherwise.
 */
bool isWaitingForTransition() {
    return waitingForTransition;
}

/**
 * @brief Retrieves the current playback mode.
 * @return The current playback mode (SEQUENCE or DIRECT).
 */
PlaybackMode getCurrentPlaybackMode() {
    return currentMode;
}

/**
 * @brief Convenience function to start sequence playback.
 */
void playSequence() {
    setPlaybackMode(SEQUENCE);
}

/**
 * @brief Convenience function to start direct file playback.
 * @param filename The file to play (can be SD card path or web URL).
 */
void playDirectFile(const String& filename) {
    setPlaybackMode(DIRECT, filename);
}

/**
 * @brief Plays a random file from the music folder on SD card.
 * @param musicFolder The folder path to search for music files (default: "/music").
 */
void playRandomFile(const String& musicFolder) {
    Serial.println("Searching for random music file in: " + musicFolder);
    
    // Check if SD card is available
    if (!SD.begin()) {
        Serial.println("ERROR: SD card not available for random file selection");
        return;
    }
    
    // Open the music folder
    File dir = SD.open(musicFolder);
    if (!dir) {
        Serial.println("ERROR: Could not open music folder: " + musicFolder);
        return;
    }
    
    if (!dir.isDirectory()) {
        Serial.println("ERROR: " + musicFolder + " is not a directory");
        dir.close();
        return;
    }
    
    // Count and collect music files
    std::vector<String> musicFiles;
    dir.rewindDirectory();
    
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break; // No more files
        }
        
        String filename = entry.name();
        String fullPath = musicFolder + "/" + filename;
        
        // Check if it's a music file (by extension)
        if (filename.endsWith(".mp3") || filename.endsWith(".MP3") ||
            filename.endsWith(".wav") || filename.endsWith(".WAV") ||
            filename.endsWith(".m4a") || filename.endsWith(".M4A") ||
            filename.endsWith(".aac") || filename.endsWith(".AAC")) {
            
            musicFiles.push_back(fullPath);
            Serial.println("Found music file: " + fullPath);
        }
        
        entry.close();
    }
    
    dir.close();
    
    // Check if we found any music files
    if (musicFiles.empty()) {
        Serial.println("ERROR: No music files found in " + musicFolder);
        return;
    }
    
    // Select a random file
    int randomIndex = random(0, musicFiles.size());
    String selectedFile = musicFiles[randomIndex];
    
    Serial.println("Randomly selected: " + selectedFile + " (file " + String(randomIndex + 1) + " of " + String(musicFiles.size()) + ")");
    
    // Play the selected file
    playDirectFile(selectedFile);
}
