/**
 * @file shuffle_manager.cpp
 * @brief Manages shuffle playback from SD card music files.
 */

#include "shuffle_manager.h"
#include "../hardware/hardware_setup.h"
#include <SD.h>

// Shuffle state
static ShuffleState shuffleState = {
    .shuffleQueue = std::vector<String>(),
    .recentlyPlayed = std::vector<String>(),
    .currentSongIndex = 0,
    .musicFolder = "/music",
    .shuffleAutoAdvance = true
};

ShuffleState& getShuffleState() {
    return shuffleState;
}

/**
 * @brief Builds shuffle queue from music folder.
 */
void buildShuffleQueue(const String& musicFolder) {
    Serial.println("Building shuffle queue from: " + musicFolder);
    
    // Clear existing queue
    shuffleState.shuffleQueue.clear();
    shuffleState.recentlyPlayed.clear();
    shuffleState.currentSongIndex = 0;
    
    // Check if SD card is available
    if (!SD.begin()) {
        Serial.println("ERROR: SD card not available for shuffle queue");
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
    
    // Scan for music files
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
            
            shuffleState.shuffleQueue.push_back(fullPath);
            Serial.println("Added to shuffle queue: " + fullPath);
        }
        
        entry.close();
    }
    
    dir.close();
    
    Serial.println("Shuffle queue built with " + String(shuffleState.shuffleQueue.size()) + " files");
}

/**
 * @brief Plays the next track in shuffle mode.
 */
void playNextShuffleTrack() {
    if (shuffleState.shuffleQueue.empty()) {
        Serial.println("Shuffle queue is empty");
        return;
    }
    
    // Select random track, avoiding recently played
    String selectedFile;
    int maxAttempts = 10;
    int attempts = 0;
    
    do {
        int randomIndex = random(0, shuffleState.shuffleQueue.size());
        selectedFile = shuffleState.shuffleQueue[randomIndex];
        attempts++;
        
        // Avoid repeating the immediate previous track only
        String lastFile = shuffleState.recentlyPlayed.empty()
                          ? String("")
                          : shuffleState.recentlyPlayed.back();
        bool isRepeat = (selectedFile == lastFile);
        
        if (!isRepeat || attempts >= maxAttempts) {
            break;
        }
    } while (attempts < maxAttempts);
    
    // Add to recently played list
    shuffleState.recentlyPlayed.push_back(selectedFile);
    
    // Keep recently played list manageable
    if (shuffleState.recentlyPlayed.size() > 5) {
        shuffleState.recentlyPlayed.erase(shuffleState.recentlyPlayed.begin());
    }
    
    // Play the selected file
    Serial.println("Playing shuffle track: " + selectedFile);
    audio.connecttoFS(SD, selectedFile.c_str());
}

/**
 * @brief Handles SHUFFLE program logic.
 */
void handleShuffleProgram() {
    // Check if current song finished and auto-advance is enabled
    if (!audio.isRunning() && shuffleState.shuffleAutoAdvance && !shuffleState.shuffleQueue.empty()) {
        playNextShuffleTrack();
    }
}

/**
 * @brief Plays a random file from the music folder on SD card.
 */
void playRandomFile(const String& musicFolder) {
    shuffleState.musicFolder = musicFolder;
    buildShuffleQueue(musicFolder);
    
    // Play first random track
    if (!shuffleState.shuffleQueue.empty()) {
        playNextShuffleTrack();
    }
}
