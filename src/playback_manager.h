/**
 * @file playback_manager.h
 * @brief Header file for managing playback modes and logic.
 */

#pragma once

#include "Arduino.h"

// Playback modes
/**
 * @enum PlaybackMode
 * @brief Defines the playback modes.
 * @var SEQUENCE Play sequence of notes with gaps/timing.
 * @var DIRECT Play single file directly.
 */
enum PlaybackMode {
    SEQUENCE,  /**< Play sequence of notes with gaps/timing. */
    DIRECT     /**< Play single file directly. */
};

// Playback management functions
/**
 * @brief Sets the playback mode and initializes playback state.
 * @param mode The playback mode (SEQUENCE or DIRECT).
 * @param file The file to play in DIRECT mode (optional, can be SD card path or web URL).
 */
void setPlaybackMode(PlaybackMode mode, const String& file = "");

/**
 * @brief Handles playback logic based on the current mode.
 */
void handlePlayback();

/**
 * @brief Checks if playback is currently active.
 * @return True if playback is active, false otherwise.
 */
bool isPlaybackActive();

/**
 * @brief Stops the current playback and resets playback state.
 */
void stopPlayback();

/**
 * @brief Retrieves the current playback mode.
 * @return The current playback mode (SEQUENCE or DIRECT).
 */
PlaybackMode getCurrentPlaybackMode();

// Legacy functions for compatibility
/**
 * @brief Handles transitions between notes in sequence playback.
 */
void handleTransitions();

/**
 * @brief Starts a transition gap before playing the next note.
 * @param url The URL or file path of the next note.
 */
void playNextNote(const String& url);

/**
 * @brief Initiates a transition to the next note.
 * @param url The URL or file path of the next note.
 */
void startTransition(const String& url);

/**
 * @brief Checks if the system is waiting for a transition gap.
 * @return True if waiting for a transition, false otherwise.
 */
bool isWaitingForTransition();

// Convenience functions for easy mode switching
/**
 * @brief Convenience function to start sequence playback.
 */
void playSequence();

/**
 * @brief Convenience function to start direct file playback.
 * @param filename The file to play (can be SD card path or web URL).
 */
void playDirectFile(const String& filename);

/**
 * @brief Plays a random file from the music folder on SD card.
 * @param musicFolder The folder path to search for music files (default: "/music").
 */
void playRandomFile(const String& musicFolder = "/music");
