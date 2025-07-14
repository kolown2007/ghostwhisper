/**
 * @file radio_manager.cpp
 * @brief Manages radio program playback modes and coordination.
 */

#include "radio_manager.h"
#include "stream_manager.h"
#include "shuffle_manager.h"
#include "generative_manager.h"
#include "hardware_setup.h"
#include "musicdata.h"
#include <SD.h>
#include <vector>

// Global program state - simplified
static ProgramState programState = {
    .currentProgram = GENERATIVE_PROGRAM,
    .programActive = false
};

// Removed legacy variables - no longer needed

/**
 * @brief Sets the radio program mode and initializes program state.
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
            buildShuffleQueue(parameter.length() > 0 ? parameter : "/music");
            programState.programActive = true;
            break;
            
        case GENERATIVE_PROGRAM: {
            Serial.println("GENERATIVE");
            playSequence(); // This sets generativeActive and programActive
            programState.programActive = true;
            break;
        }
            
        case STREAM_PROGRAM:
            Serial.println("STREAM");
            if (parameter.length() > 0) {
                connectToStream(parameter);
            } else {
                String defaultURL = getDefaultStreamURL();
                Serial.println("DEBUG: Default stream URL from musicdata.h: " + defaultURL);
                connectToStream(defaultURL);
            }
            programState.programActive = isStreamConnected();
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
 */
ProgramState& getProgramState() {
    return programState;
}

/**
 * @brief Gets the current radio program.
 */
RadioProgram getCurrentProgram() {
    return programState.currentProgram;
}

/**
 * @brief Checks if any program is currently active.
 */
bool isPlaybackActive() {
    return programState.programActive;
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
    
    // Reset module states
    getGenerativeState().generativeActive = false;
    getStreamState().streamConnected = false;
}

// Legacy compatibility functions

/**
 * @brief Legacy function - sets playback mode (maps to new program system).
 */
void setPlaybackMode(PlaybackMode mode, const String& file) {
    if (mode == SEQUENCE) {
        setProgramMode(GENERATIVE_PROGRAM);
    } else if (mode == DIRECT) {
        if (file.length() > 0) {
            if (file.startsWith("/")) {
                setProgramMode(SHUFFLE_PROGRAM);
                // Play the specific file directly
                audio.connecttoFS(SD, file.c_str());
                programState.programActive = true;
            } else {
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
    handleProgramPlayback();
}

/**
 * @brief Legacy function - gets current playback mode.
 */
PlaybackMode getCurrentPlaybackMode() {
    switch (programState.currentProgram) {
        case GENERATIVE_PROGRAM:
            return SEQUENCE;
        case SHUFFLE_PROGRAM:
        case STREAM_PROGRAM:
        default:
            return DIRECT;
    }
}

// Convenience functions

/**
 * @brief Convenience function to start direct file playback.
 */
void playDirectFile(const String& filename) {
    if (filename.startsWith("/")) {
        // SD card file
        setProgramMode(SHUFFLE_PROGRAM);
        Serial.println("Playing direct file from SD: " + filename);
        audio.connecttoFS(SD, filename.c_str());
        programState.programActive = true;
    } else {
        // Web URL - use stream program
        setProgramMode(STREAM_PROGRAM, filename);
    }
}
