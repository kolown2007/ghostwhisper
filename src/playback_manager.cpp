/**
 * @file playback_manager.cpp
 * @brief Manages radio program playback modes and logic.
 */

#include "playback_manager.h"
#include "hardware_setup.h"
#include "musicdata.h"
#include <SD.h>
#include <vector>

// Global program state
static ProgramState programState = {
    .currentProgram = GENERATIVE_PROGRAM,
    .programActive = false,
    
    // SHUFFLE state
    .shuffleQueue = std::vector<String>(),
    .recentlyPlayed = std::vector<String>(),
    .currentSongIndex = 0,
    .musicFolder = "/music",
    .shuffleAutoAdvance = true,
    
    // GENERATIVE state
    .generativeActive = false,
    .lastNoteTime = 0,
    .nextNoteDelay = 0,
    .currentNoteSequence = "",
    .currentNoteIndex = 0,
    .waitingForTransition = false,
    .nextNoteUrl = "",
    
    // STREAM state
    .currentStreamURL = "",
    .streamConnected = false,
    .reconnectAttempts = 0,
    .availableStreams = std::vector<String>()
};

// Legacy variables for backward compatibility
static PlaybackMode currentMode = SEQUENCE;
static String directPlayFile = "";
static bool playbackActive = false;
static bool directPlayStarted = false;

// Forward declarations
void handleShuffleProgram();
void handleGenerativeProgram();
void handleStreamProgram();
void buildShuffleQueue(const String& musicFolder);
void playNextShuffleTrack();
void connectToStream(const String& url);
void handleStreamReconnection();
void tryNextStream(); // Forward declaration of the new function

/**
 * @brief Sets the radio program mode and initializes program state.
 * @param program The radio program (SHUFFLE_PROGRAM, GENERATIVE_PROGRAM, or STREAM_PROGRAM).
 * @param parameter Optional parameter (folder path for SHUFFLE, URL for STREAM).
 */
void setProgramMode(RadioProgram program, const String& parameter) {
    // Stop current program
    stopPlayback();
    
    // Set new program
    programState.currentProgram = program;
    programState.programActive = false;
    
    Serial.print("Program mode set to: ");
    switch (program) {
        case SHUFFLE_PROGRAM:
            Serial.println("SHUFFLE");
            if (parameter.length() > 0) {
                programState.musicFolder = parameter;
            }
            buildShuffleQueue(programState.musicFolder);
            break;
            
        case GENERATIVE_PROGRAM: {
            Serial.println("GENERATIVE");
            programState.generativeActive = true;
            programState.waitingForTransition = false;
            programState.programActive = true;

            // Load bell sounds to make sure they're available
            const std::vector<String>& bellSounds = getBellSounds();
            Serial.println("Generative program activated with " + String(bellSounds.size()) + " bell sounds");
            break;
        }
            
        case STREAM_PROGRAM:
            Serial.println("STREAM");
            if (parameter.length() > 0) {
                programState.currentStreamURL = parameter;
                connectToStream(parameter);
            } else {
                // Use default stream URL from musicdata.h
                String defaultURL = getDefaultStreamURL();
                Serial.println("DEBUG: Default stream URL from musicdata.h: " + defaultURL);
                programState.currentStreamURL = defaultURL;
                connectToStream(programState.currentStreamURL);
            }
            break;
    }
    
    Serial.println("Program initialization complete");
}

/**
 * @brief Handles playback logic based on the current program.
 */
void handleProgramPlayback() {
    switch (programState.currentProgram) {
        case SHUFFLE_PROGRAM:
            handleShuffleProgram();
            break;
            
        case GENERATIVE_PROGRAM:
            handleGenerativeProgram();
            break;
            
        case STREAM_PROGRAM:
            handleStreamProgram();
            break;
    }
}

/**
 * @brief Gets the current program state.
 * @return Reference to the current program state.
 */
ProgramState& getProgramState() {
    return programState;
}

/**
 * @brief Gets the current radio program.
 * @return The current radio program.
 */
RadioProgram getCurrentProgram() {
    return programState.currentProgram;
}

/**
 * @brief Handles SHUFFLE program logic.
 */
void handleShuffleProgram() {
    // Check if current song finished and auto-advance is enabled
    if (!audio.isRunning() && programState.shuffleAutoAdvance && !programState.shuffleQueue.empty()) {
        playNextShuffleTrack();
    }
}

/**
 * @brief Handles GENERATIVE program logic.
 */
void handleGenerativeProgram() {
    static unsigned long lastNoteTime = 0;
    static unsigned long nextNoteDelay = 2000; // Start with 2 second delay
    
    // Check if it's time to play the next note
    if (programState.generativeActive && 
        (millis() - lastNoteTime >= nextNoteDelay || !audio.isRunning())) {
        
        // Get bell sounds from musicbox folder
        const std::vector<String>& bellSounds = getBellSounds();
        
        if (bellSounds.size() > 0) {
            // Pick a random bell sound
            int randomIndex = random(0, bellSounds.size());
            String selectedSound = bellSounds[randomIndex];
            
            Serial.println("Playing generative note: " + selectedSound);
            
            // Play the sound from SD card
            if (audio.connecttoFS(SD, selectedSound.c_str())) {
                lastNoteTime = millis();
                
                // Random delay between 1-5 seconds for next note
                nextNoteDelay = random(1000, 5000);
                
                Serial.println("Next note in " + String(nextNoteDelay/1000) + " seconds");
            } else {
                Serial.println("Failed to play: " + selectedSound);
                // Try again sooner if failed
                nextNoteDelay = 500;
            }
        } else {
            Serial.println("No bell sounds found in /musicbox folder");
            // Check again in 5 seconds
            lastNoteTime = millis();
            nextNoteDelay = 5000;
        }
    }
}

/**
 * @brief Handles STREAM program logic.
 */
void handleStreamProgram() {
    static unsigned long lastErrorCheck = 0;
    static int consecutiveErrors = 0;
    
    // Monitor stream connection
    if (!programState.streamConnected && programState.reconnectAttempts < 3) {
        handleStreamReconnection();
        return;
    }
    
    // Check if stream is still active
    if (programState.streamConnected && !audio.isRunning()) {
        Serial.println("Stream disconnected, attempting reconnection");
        programState.streamConnected = false;
        programState.reconnectAttempts++;
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
 * @brief Builds shuffle queue from music folder.
 * @param musicFolder The folder path to scan for music files.
 */
void buildShuffleQueue(const String& musicFolder) {
    Serial.println("Building shuffle queue from: " + musicFolder);
    
    // Clear existing queue
    programState.shuffleQueue.clear();
    programState.recentlyPlayed.clear();
    programState.currentSongIndex = 0;
    
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
            
            programState.shuffleQueue.push_back(fullPath);
            Serial.println("Added to shuffle queue: " + fullPath);
        }
        
        entry.close();
    }
    
    dir.close();
    
    Serial.println("Shuffle queue built with " + String(programState.shuffleQueue.size()) + " files");
}

/**
 * @brief Plays the next track in shuffle mode.
 */
void playNextShuffleTrack() {
    if (programState.shuffleQueue.empty()) {
        Serial.println("Shuffle queue is empty");
        return;
    }
    
    // Select random track, avoiding recently played
    String selectedFile;
    int maxAttempts = 10;
    int attempts = 0;
    
    do {
        int randomIndex = random(0, programState.shuffleQueue.size());
        selectedFile = programState.shuffleQueue[randomIndex];
        attempts++;
        
        // Avoid repeating the immediate previous track only
        String lastFile = programState.recentlyPlayed.empty()
                          ? String("")
                          : programState.recentlyPlayed.back();
        bool isRepeat = (selectedFile == lastFile);
        
        if (!isRepeat || attempts >= maxAttempts) {
            break;
        }
    } while (attempts < maxAttempts);
    
    // Add to recently played list
    programState.recentlyPlayed.push_back(selectedFile);
    
    // Keep recently played list manageable
    if (programState.recentlyPlayed.size() > 5) {
        programState.recentlyPlayed.erase(programState.recentlyPlayed.begin());
    }
    
    // Play the selected file
    Serial.println("Playing shuffle track: " + selectedFile);
    audio.connecttoFS(SD, selectedFile.c_str());
    programState.programActive = true;
}

/**
 * @brief Connects to a stream URL.
 * @param url The stream URL to connect to.
 */
void connectToStream(const String& url) {
    Serial.println("=== STREAM CONNECTION ATTEMPT ===");
    Serial.println("Connecting to stream: " + url);
    
    // Stop any current playback first
    audio.stopSong();
    delay(500); // Increased delay for better cleanup
    
    programState.currentStreamURL = url;
    programState.streamConnected = false;
    programState.reconnectAttempts = 0;
    
    Serial.println("Attempting to connect to host...");
    
    // Set audio buffer size for better streaming (if available)
    // audio.setConnectionTimeout(10000, 3000); // Uncomment if method exists
    
    // Attempt to connect
    if (audio.connecttohost(url.c_str())) {
        programState.streamConnected = true;
        programState.programActive = true;
        Serial.println("✓ Stream connected successfully!");
        Serial.println("Audio volume: " + String(audio.getVolume()));
        
        // Give some time for stream to stabilize
        delay(1000);
    } else {
        Serial.println("✗ Failed to connect to stream");
        programState.reconnectAttempts++;
    }
    
    Serial.println("=== END STREAM CONNECTION ===");
}

/**
 * @brief Handles stream reconnection logic.
 */
void handleStreamReconnection() {
    if (programState.currentStreamURL.length() > 0 && programState.reconnectAttempts < 3) {
        Serial.println("Attempting stream reconnection (" + String(programState.reconnectAttempts + 1) + "/3)");
        
        // Stop current connection cleanly
        audio.stopSong();
        delay(2000); // Wait longer before retry to let network stabilize
        
        // Clear any audio buffers
        Serial.println("Clearing audio buffers before reconnection...");
        
        if (audio.connecttohost(programState.currentStreamURL.c_str())) {
            programState.streamConnected = true;
            programState.programActive = true;
            Serial.println("Stream reconnected successfully");
            
            // Give stream time to stabilize
            delay(1500);
        } else {
            programState.reconnectAttempts++;
            Serial.println("Stream reconnection failed (attempt " + String(programState.reconnectAttempts) + ")");
            
            // If all attempts failed, reset and try default URL
            if (programState.reconnectAttempts >= 3) {
                Serial.println("All reconnection attempts failed, trying default stream...");
                String defaultURL = getDefaultStreamURL();
                if (defaultURL != programState.currentStreamURL) {
                    programState.currentStreamURL = defaultURL;
                    programState.reconnectAttempts = 0; // Reset attempts for new URL
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
    
    // Get current stream index
    int currentIndex = -1;
    for (int i = 0; i < availableStreams.size(); i++) {
        if (availableStreams[i].url == programState.currentStreamURL) {
            currentIndex = i;
            break;
        }
    }
    
    // Move to next stream
    int nextIndex = (currentIndex + 1) % availableStreams.size();
    String nextURL = availableStreams[nextIndex].url;
    String nextName = availableStreams[nextIndex].name;
    
    Serial.println("Switching from index " + String(currentIndex) + " to " + String(nextIndex));
    Serial.println("New stream: " + nextName + " (" + nextURL + ")");
    
    // Connect to new stream
    connectToStream(nextURL);
    
    Serial.println("=== STREAM SWITCH COMPLETE ===");
}

// Force clear any cached stream configuration
void clearStreamCache() {
    Serial.println("=== CLEARING STREAM CACHE ===");
    
    // Stop any current audio first
    audio.stopSong();
    delay(500);
    
    // Reset program state completely
    programState.currentStreamURL = "";
    programState.streamConnected = false;
    programState.reconnectAttempts = 0;
    programState.programActive = false;
    programState.generativeActive = false;
    programState.waitingForTransition = false;
    
    // Clear any static variables that might cache the URL
    // Force reload default URL from musicdata.h
    String newURL = getDefaultStreamURL();
    Serial.println("Forced reload - new default URL: " + newURL);
    programState.currentStreamURL = newURL;
    
    // Force garbage collection
    delay(100);
    
    Serial.println("=== STREAM CACHE CLEARED ===");
}

// Legacy and compatibility functions

/**
 * @brief Checks if any program is currently active.
 * @return True if any program is active, false otherwise.
 */
bool isPlaybackActive() {
    return programState.programActive || playbackActive; // Support both new and legacy
}

/**
 * @brief Stops the current program and resets state.
 */
void stopPlayback() {
    Serial.println("Stopping playback");
    
    // Stop audio
    audio.stopSong();
    
    // Reset program state
    programState.programActive = false;
    programState.generativeActive = false;
    programState.waitingForTransition = false;
    programState.streamConnected = false;
    
    // Reset legacy state
    playbackActive = false;
    directPlayStarted = false;
}

/**
 * @brief Legacy function - sets playback mode (maps to new program system).
 * @param mode The playback mode (SEQUENCE or DIRECT).
 * @param file The file to play in DIRECT mode (optional).
 */
void setPlaybackMode(PlaybackMode mode, const String& file) {
    // Update legacy variables
    currentMode = mode;
    directPlayFile = file;
    playbackActive = false;
    directPlayStarted = false;
    
    // Map to new program system
    if (mode == SEQUENCE) {
        setProgramMode(GENERATIVE_PROGRAM);
    } else if (mode == DIRECT) {
        if (file.length() > 0) {
            if (file.startsWith("/")) {
                // SD card file - treat as single file shuffle
                setProgramMode(SHUFFLE_PROGRAM);
                playDirectFile(file);
            } else {
                // Web URL - treat as stream
                setProgramMode(STREAM_PROGRAM, file);
            }
        }
    }
    
    Serial.print("Legacy playback mode set to: ");
    Serial.println(mode == SEQUENCE ? "SEQUENCE" : "DIRECT");
}

/**
 * @brief Legacy function - handles playback (maps to new program system).
 */
void handlePlayback() {
    // Handle new program system
    handleProgramPlayback();
    
    // Handle legacy direct playback if still needed
    if (currentMode == DIRECT && !directPlayStarted && directPlayFile.length() > 0) {
        Serial.println("Starting legacy direct playback: " + directPlayFile);
        
        if (directPlayFile.startsWith("/")) {
            audio.connecttoFS(SD, directPlayFile.c_str());
        } else {
            audio.connecttohost(directPlayFile.c_str());
        }
        
        directPlayStarted = true;
        playbackActive = true;
    }
}

/**
 * @brief Legacy function - gets current playback mode.
 * @return The current playback mode.
 */
PlaybackMode getCurrentPlaybackMode() {
    // Map from new program system to legacy
    switch (programState.currentProgram) {
        case GENERATIVE_PROGRAM:
            return SEQUENCE;
        case SHUFFLE_PROGRAM:
        case STREAM_PROGRAM:
        default:
            return DIRECT;
    }
}

// Legacy GENERATIVE program functions (formerly SEQUENCE mode)

/**
 * @brief Handles transitions between notes in generative playback.
 */
void handleTransitions() {
    // Simple implementation: just play a random bell sound
    const std::vector<String>& bellSounds = getBellSounds();
    
    if (bellSounds.size() > 0) {
        int randomIndex = random(0, bellSounds.size());
        String selectedSound = bellSounds[randomIndex];
        
        Serial.println("Transition: Playing bell sound: " + selectedSound);
        audio.connecttoFS(SD, selectedSound.c_str());
    } else {
        Serial.println("No bell sounds available for transition");
    }
}

/**
 * @brief Starts ambient transition to play the next note with overlap.
 * @param url The URL or file path of the next note.
 */
void playNextNote(const String& url) {
    // Start ambient transition: store URL and trigger immediate overlapping playback
    Serial.println("Starting ambient transition for next note");
    programState.nextNoteUrl = url;
    programState.waitingForTransition = true;
    programState.lastNoteTime = millis();
}

/**
 * @brief Initiates a transition to the next note.
 * @param url The URL or file path of the next note.
 */
void startTransition(const String& url) {
    playNextNote(url);
}

/**
 * @brief Checks if the system is waiting for a seamless transition.
 * @return True if waiting for a transition, false otherwise.
 */
bool isWaitingForTransition() {
    return programState.waitingForTransition;
}

// Convenience functions

/**
 * @brief Convenience function to start generative (sequence) playback.
 */
void playSequence() {
    setProgramMode(GENERATIVE_PROGRAM);
}

/**
 * @brief Convenience function to start direct file playback.
 * @param filename The file to play (can be SD card path or web URL).
 */
void playDirectFile(const String& filename) {
    if (filename.startsWith("/")) {
        // SD card file - use shuffle program for single file
        setProgramMode(SHUFFLE_PROGRAM);
        Serial.println("Playing direct file from SD: " + filename);
        audio.connecttoFS(SD, filename.c_str());
        programState.programActive = true;
    } else {
        // Web URL - use stream program
        setProgramMode(STREAM_PROGRAM, filename);
    }
}

/**
 * @brief Plays a random file from the music folder on SD card.
 * @param musicFolder The folder path to search for music files (default: "/music").
 */
void playRandomFile(const String& musicFolder) {
    // Set to shuffle mode and build queue
    setProgramMode(SHUFFLE_PROGRAM, musicFolder);
    
    // Play first random track
    if (!programState.shuffleQueue.empty()) {
        playNextShuffleTrack();
    }
}
