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
    // Configure watchdog with longer timeout for setup
    esp_task_wdt_init(30, true); // 30 second timeout during setup
    esp_task_wdt_add(NULL);
    
    initializeHardware();
    randomSeed(esp_random()); // Seed with ESP32 hardware random generator
    
    // Reset watchdog before connection init
    esp_task_wdt_reset();
    
    initializeConnection(ONLINE);  // Change to OFFLINE for no WiFi

    // Reset watchdog before web init
    esp_task_wdt_reset();

    // Initialize web control interface for both ONLINE and OFFLINE modes
    // In OFFLINE mode, it creates local AP for web access
    initializeWebControl();
    
    // Reset watchdog before stream data fetch
    esp_task_wdt_reset();
    
    // Stream data initialization removed for now
    
    // Reset watchdog before program init
    esp_task_wdt_reset();

    // Set default program mode to generative (ambient music playback)
    setProgramMode(GENERATIVE_PROGRAM, "");
    
    // Reconfigure watchdog for normal operation (shorter timeout)
    esp_task_wdt_init(10, true); // 10 second timeout during normal operation
    esp_task_wdt_add(NULL);
    
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

