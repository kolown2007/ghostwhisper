/**
 * @file playback_manager.h
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

// Program state structure
/**
 * @struct ProgramState
 * @brief Holds the state for all radio programs.
 */
struct ProgramState {
    RadioProgram currentProgram;
    bool programActive;
    
    // SHUFFLE state
    std::vector<String> shuffleQueue;
    std::vector<String> recentlyPlayed;
    int currentSongIndex;
    String musicFolder;
    bool shuffleAutoAdvance;
    
    // GENERATIVE state (note-based system)
    bool generativeActive;
    unsigned long lastNoteTime;
    unsigned long nextNoteDelay;
    String currentNoteSequence;
    int currentNoteIndex;
    bool waitingForTransition;
    String nextNoteUrl;
    
    // STREAM state
    String currentStreamURL;
    bool streamConnected;
    int reconnectAttempts;
    std::vector<String> availableStreams;
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

// Program-specific functions
/**
 * @brief Handles SHUFFLE program logic.
 */
void handleShuffleProgram();

/**
 * @brief Handles GENERATIVE program logic.
 */
void handleGenerativeProgram();

/**
 * @brief Handles STREAM program logic.
 */
void handleStreamProgram();

/**
 * @brief Builds shuffle queue from music folder.
 * @param musicFolder The folder path to scan for music files.
 */
void buildShuffleQueue(const String& musicFolder);

/**
 * @brief Plays the next track in shuffle mode.
 */
void playNextShuffleTrack();

/**
 * @brief Connects to a stream URL.
 * @param url The stream URL to connect to.
 */
void connectToStream(const String& url);

/**
 * @brief Handles stream reconnection logic.
 */
void handleStreamReconnection();

/**
 * @brief Try the next available stream if current one has issues.
 */
void tryNextStream();

// Legacy and compatibility functions
/**
 * @brief Checks if any program is currently active.
 * @return True if any program is active, false otherwise.
 */
bool isPlaybackActive();

/**
 * @brief Stops the current program and resets state.
 */
void stopPlayback();

/**
 * @brief Force clear any cached stream configuration.
 */
void clearStreamCache();

/**
 * @brief Legacy function - sets playback mode (maps to new program system).
 * @param mode The playback mode (SEQUENCE or DIRECT).
 * @param file The file to play in DIRECT mode (optional).
 */
void setPlaybackMode(PlaybackMode mode, const String& file = "");

/**
 * @brief Legacy function - handles playback (maps to new program system).
 */
void handlePlayback();

/**
 * @brief Legacy function - gets current playback mode.
 * @return The current playback mode.
 */
PlaybackMode getCurrentPlaybackMode();

// Legacy GENERATIVE program functions (formerly SEQUENCE mode)
/**
 * @brief Handles transitions between notes in generative playback.
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

// Convenience functions
/**
 * @brief Convenience function to start generative (sequence) playback.
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
