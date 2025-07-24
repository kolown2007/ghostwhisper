/**
 * @file volume_control.h
 * @brief Volume control functionality
 * @details Manages audio volume settings and control
 */

#pragma once

#include "Arduino.h"

/**
 * @brief Get current volume level
 * @return Current volume percentage (0-100)
 */
int getCurrentVolume();

/**
 * @brief Set volume level
 * @param volume Volume level (0-100)
 * @return true if volume was set successfully
 */
bool setVolume(int volume);

/**
 * @brief Increase volume by step amount
 * @return New volume level
 */
int increaseVolume();

/**
 * @brief Decrease volume by step amount
 * @return New volume level
 */
int decreaseVolume();

/**
 * @brief Synchronize volume with audio library
 */
void syncVolumeWithAudio();

/**
 * @brief Test volume control functionality
 */
void testVolumeControl();

/**
 * @brief Initialize volume control system
 */
void initializeVolumeControl();
