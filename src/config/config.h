/**
 * @file config.h
 * @brief Configuration constants and settings for GhostWhisper system
 */

#pragma once

// Hardware Configuration
// SD card pins are kept for reference but SD usage is disabled in online mode.
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK 18
#define SD_CS 5

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

// LED Configuration
#define STATUS_LED_PIN LED_BUILTIN  // Use built-in LED for status indication

// System Configuration
#define SERIAL_BAUD_RATE 9600
#define WATCHDOG_TIMEOUT_SEC 30
#define WIFI_TIMEOUT_SEC 180

// Audio Configuration
#define DEFAULT_VOLUME 30
#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define VOLUME_STEP 1

// Web Server Configuration
#define WEB_SERVER_PORT 80
#define WEB_FILES_PATH "/view"

// Music Configuration
#define MUSIC_FOLDER "/music"

// Debug Configuration
#define DEBUG_INTERVAL_MS 30000
#define HEALTH_CHECK_INTERVAL_MS 30000
#define LOW_MEMORY_THRESHOLD 10000

// Connection Configuration
// Default to ONLINE mode for this branch (no local SD/offline AP usage)
#define DEFAULT_CONNECTION_MODE ONLINE  // ONLINE or OFFLINE
#define CLEAR_WIFI_ON_STARTUP false      // Set to true to clear WiFi credentials on startup

// Feature toggles
// Disable SD to reduce firmware size and remove SD-related runtime code paths
#define ENABLE_SD 0
// Disable offline AP/config portal in this online-only build
#define ENABLE_OFFLINE_AP 0

// mDNS responder: set to 1 to enable printing/starting mDNS (ghostwhisper.local).
// Default is 0 to avoid confusing users on platforms that don't resolve .local names.
#define ENABLE_MDNS 0

// WiFi credentials are now defined in secrets.h
