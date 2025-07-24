/**
 * @file control.h
 * @brief Header file for web server control interface.
 * @details Provides web-based control for volume and system monitoring.
 */

#pragma once

#include "Arduino.h"

/**
 * @brief Initialize the web server control interface.
 * @details Sets up HTTP routes for volume control and system status.
 */
void initializeWebControl();

/**
 * @brief Handle incoming web server requests.
 * @details Must be called in the main loop to process HTTP requests.
 */
void handleWebControl();
