#pragma once

#include "Arduino.h"

// Playback management functions
void handleTransitions();
void playNextNote(const String& url);
void startTransition(const String& url);
bool isWaitingForTransition();

// Playback state functions
void incrementNoteCounter();
