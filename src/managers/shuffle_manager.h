#pragma once

#include "Arduino.h"
#include <vector>

// Shuffle playback functions
void buildShuffleQueue(const String& musicFolder);
void playNextShuffleTrack();
void handleShuffleProgram();
void playRandomFile(const String& musicFolder = "/music");

// Shuffle state management
struct ShuffleState {
    std::vector<String> shuffleQueue;
    std::vector<String> recentlyPlayed;
    int currentSongIndex;
    String musicFolder;
    bool shuffleAutoAdvance;
};

ShuffleState& getShuffleState();
