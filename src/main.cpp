#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "managers/connection_manager.h"
#include "managers/radio_manager.h"
#include "managers/debug_manager.h"
#include "web/control.h"
#include <esp_task_wdt.h>
#include <esp_random.h>


void setup() {
  
   
    
    initializeHardware();
    randomSeed(esp_random()); // Seed with ESP32 hardware random generator
    
   initializeConnection(OFFLINE);  // Change to OFFLINE for no WiFi

    // Initialize web control interface for both ONLINE and OFFLINE modes
    // In OFFLINE mode, it creates local AP for web access
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
    
    // Handle web server requests for both ONLINE and OFFLINE modes
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

