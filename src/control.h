/**
 * @file control.h
 * @brief Header file for web server volume control interface.
 * @details Provides web-based control for volume and system monitoring.
 */

#pragma once

#include "Arduino.h"
#include <WebServer.h>

/**
 * @brief Initialize the web server volume control interface.
 * @details Sets up HTTP routes for volume control and system status.
 */
void initializeWebControl();

/**
 * @brief Handle incoming web server requests.
 * @details Must be called in the main loop to process HTTP requests.
 */
void handleWebControl();

/**
 * @brief Check if the web server is running.
 * @return True if web server is active, false otherwise.
 */
bool isWebServerActive();

/**
 * @brief Get the current web server status.
 * @return String containing server status information.
 */
String getWebServerStatus();

/**
 * @brief Stop the web server.
 */
void stopWebServer();

/**
 * @brief Restart the web server.
 */
void restartWebServer();

/**
 * @brief Test audio volume control functionality.
 * @details Runs diagnostic tests to verify volume control is working.
 */
void testVolumeControl();

/**
 * @brief Handle volume test web request.
 * @details Web endpoint to trigger volume diagnostic tests.
 */
void handleVolumeTest();
