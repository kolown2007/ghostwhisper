/**
 * @file web_utils.h
 * @brief Web utility functions
 * @details Common utility functions for web operations
 */

#pragma once

#include "Arduino.h"

/**
 * @brief Load HTML file from SD card with template processing
 * @param filename HTML file name to load
 * @return Processed HTML content
 */
String loadHTMLFromSD(const char* filename);

/**
 * @brief Generate main control page HTML
 * @return HTML content for main page
 */
String generateMainPage();

/**
 * @brief Generate system status JSON
 * @return JSON string with system status
 */
String generateStatusJson();

/**
 * @brief Check memory limits and log report
 */
void checkMemoryLimits();
