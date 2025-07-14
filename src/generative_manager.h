#pragma once

#include "Arduino.h"

// Generative playback functions
void handleGenerativeProgram();
void playSequence();

// Generative state management
struct GenerativeState {
    bool generativeActive;
    unsigned long lastNoteTime;
    unsigned long nextNoteDelay;
};

GenerativeState& getGenerativeState();
