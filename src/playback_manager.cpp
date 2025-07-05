#include "playback_manager.h"
#include "hardware_setup.h"
#include "sequence_manager.h"

// Static variables for playback management
static unsigned long lastNotePlayTime = 0;
static unsigned long noteTransitionTime = 0;
static bool waitingForTransition = false;
static String nextNoteUrl = "";
static size_t noteCounter = 0;

void handleTransitions() {
    unsigned long now = millis();
    
    // Handle transition delays with proper gap before playing
    if (waitingForTransition) {
        if (now - noteTransitionTime >= 200) { // 200ms gap before playing next note
            waitingForTransition = false;
            
            // Now play the note after the gap
            Serial.println("Gap complete, playing: " + nextNoteUrl);
            audio.connecttohost(nextNoteUrl.c_str());
            lastNotePlayTime = now;
            advanceToNextNote();
            
            Serial.println("Started note " + String(getCurrentNoteIndex()) + "/" + String(getSequenceSize()));
        }
    }
}

void playNextNote(const String& url) {
    // Start transition: store URL and begin gap period
    Serial.println("Starting transition gap for next note");
    nextNoteUrl = url;
    waitingForTransition = true;
    noteTransitionTime = millis();
}

void startTransition(const String& url) {
    playNextNote(url);
}

bool isWaitingForTransition() {
    return waitingForTransition;
}

void incrementNoteCounter() {
    noteCounter++;
}
