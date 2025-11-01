#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Audio.h>

// Function to initialize hardware components
void initializeHardware();

// Extern declaration for global audio object
extern Audio audio;
