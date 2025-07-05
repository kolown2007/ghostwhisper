#pragma once

#include "Arduino.h"
#include "sequencer.h"
#include <vector>

// Sequence management functions
void initializeSequence();
void updateSequence();
bool isSequenceInitialized();
bool isSequenceComplete();
void regenerateSequence();

// Getters for sequence state
size_t getCurrentNoteIndex();
const NoteEvent& getCurrentNote();
unsigned long getSequenceStartTime();

// Function to advance to next note
void advanceToNextNote();

// Function to get sequence size
size_t getSequenceSize();
