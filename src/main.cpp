#include "Arduino.h"
#include "config.h"
#include "hardware_setup.h"
#include "radio_manager.h"
#include "debug_manager.h"
#include "control.h"
#include <esp_task_wdt.h>
#include <esp_random.h>


void setup() {
    initializeHardware();
    randomSeed(esp_random()); // Seed with ESP32 hardware random generator
    setupAudioCallbacks();
    
    // Initialize web control interface (after hardware setup)
    initializeWebControl();
    
  

    // Set default program mode to generative (ambient music playback)
    setProgramMode(GENERATIVE_PROGRAM, "");
    
    Serial.println("Setup complete, starting program in loop...");
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();
    
    // CRITICAL: Audio processing must be first and frequent
    audio.loop();
    
    // Handle web server requests (lightweight)
    handleWebControl();
    
    // Handle program playback (new system)
    handleProgramPlayback();
    
    // Reduced frequency debug and health checks
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}

