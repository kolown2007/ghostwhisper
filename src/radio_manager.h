/**
 * @file radio_manager.h
 * @brief Header file for managing radio program playback modes.
 */

#pragma once

#include "Arduino.h"
#include <vector>

// Radio program modes
/**
 * @enum RadioProgram
 * @brief Defines the radio program modes.
 * @var SHUFFLE_PROGRAM Random shuffling of MP3 files from SD card.
 * @var GENERATIVE_PROGRAM Note-based soundfont generation with sequences.
 * @var STREAM_PROGRAM HTTP streaming from remote servers.
 */
enum RadioProgram {
    SHUFFLE_PROGRAM,    /**< Random shuffling of MP3 files from SD card. */
    GENERATIVE_PROGRAM, /**< Note-based soundfont generation with sequences. */
    STREAM_PROGRAM      /**< HTTP streaming from remote servers. */
};

// Legacy enum for backward compatibility
enum PlaybackMode {
    SEQUENCE = GENERATIVE_PROGRAM,
    DIRECT = SHUFFLE_PROGRAM
};

// Program state structure - simplified since modules handle their own state
/**
 * @struct ProgramState
 * @brief Holds the core radio program state.
 */
struct ProgramState {
    RadioProgram currentProgram;
    bool programActive;
};

// Program management functions
/**
 * @brief Sets the radio program mode and initializes program state.
 * @param program The radio program (SHUFFLE_PROGRAM, GENERATIVE_PROGRAM, or STREAM_PROGRAM).
 * @param parameter Optional parameter (folder path for SHUFFLE, URL for STREAM).
 */
void setProgramMode(RadioProgram program, const String& parameter = "");

/**
 * @brief Handles playback logic based on the current program.
 */
void handleProgramPlayback();

/**
 * @brief Gets the current program state.
 * @return Reference to the current program state.
 */
ProgramState& getProgramState();

/**
 * @brief Gets the current radio program.
 * @return The current radio program.
 */
RadioProgram getCurrentProgram();

// Legacy and compatibility functions
/**
 * @brief Checks if any program is currently active.
 */
bool isPlaybackActive();

/**
 * @brief Stops the current program and resets state.
 */
void stopPlayback();

/**
 * @brief Legacy function - sets playback mode (maps to new program system).
 */
void setPlaybackMode(PlaybackMode mode, const String& file = "");

/**
 * @brief Legacy function - handles playback (maps to new program system).
 */
void handlePlayback();

/**
 * @brief Legacy function - gets current playback mode.
 */
PlaybackMode getCurrentPlaybackMode();

// Convenience functions
/**
 * @brief Convenience function to start direct file playback.
 */
void playDirectFile(const String& filename);
