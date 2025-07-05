#include "Arduino.h"
#include "hardware_setup.h"
#include "sequence_manager.h"
#include "playback_manager.h"
#include "debug_manager.h"

unsigned long lastSoundTime = 0;
int soundCounter = 0;
bool inAmbientMode = true;

void setup() {
    initializeHardware();
    randomSeed(analogRead(A0)); // Seed random generator
    setupAudioCallbacks();
    Serial.println("Setup complete, starting sequence in loop...");
}

void loop() {
    audio.loop();
    logAudioStatus();
    
    if (!isSequenceInitialized()) {
        initializeSequence();
    }
    
    handleTransitions();
    
    if (!isWaitingForTransition() && !isSequenceComplete()) {
        unsigned long now = millis();
        const NoteEvent& evt = getCurrentNote();
        
        if (now - getSequenceStartTime() >= evt.startTimeMs) {
            startTransition(evt.filename);
        }
    }
    
    updateSequence();
}

