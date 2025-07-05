#include "hardware_setup.h"
#include "secrets.h" // For WiFi credentials

// Pin definitions (adjust as needed)
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18
#define SD_CS 5

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26
Audio audio;

void initializeHardware() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for Serial to initialize
    }

    // Initialize WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_ssid, WIFI_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("WiFi connected.");

    // Initialize SD card with SPI pins
    Serial.println("Initializing SD card...");
    SPIClass spiSD(VSPI);
    spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, spiSD)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    // Initialize audio with I2S pinout
    Serial.println("Initializing audio...");
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(80); // Set default volume
    Serial.println("Audio initialized.");
}
