#include "Arduino.h"
#include "config.h"
#include "hardware_setup.h"
#include "sequence_manager.h"
#include "playback_manager.h"
#include "debug_manager.h"
#include "control.h"
#include <esp_task_wdt.h>

unsigned long lastSoundTime = 0;
int soundCounter = 0;
bool inAmbientMode = true;

void setup() {
    initializeHardware();
    randomSeed(analogRead(A0)); // Seed random generator
    setupAudioCallbacks();
    
    // Initialize web control interface (after hardware setup)
    initializeWebControl();
    
    // Set default playback mode to sequence
    // setPlaybackMode(SEQUENCE);

    // Play a random file from the music folder
    playRandomFile(MUSIC_FOLDER);
    
    Serial.println("Setup complete, starting sequence in loop...");
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();
    
    // CRITICAL: Audio processing must be first and frequent
    audio.loop();
    
    // Handle web server requests (lightweight)
    handleWebControl();
    
    // Handle playback (sequence or direct mode)
    handlePlayback();
    
    // Reduced frequency debug and health checks
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
    
    // Only initialize and handle sequence logic if in sequence mode
    if (getCurrentPlaybackMode() == SEQUENCE) {
        if (!isSequenceInitialized()) {
            initializeSequence();
        }
        
        if (!isWaitingForTransition() && !isSequenceComplete()) {
            unsigned long now = millis();
            const NoteEvent& evt = getCurrentNote();
            
            if (now - getSequenceStartTime() >= evt.startTimeMs) {
                startTransition(evt.filename);
            }
        }
        
        updateSequence();
    }
    
    // REMOVED: No delay in main loop for audio performance
}

