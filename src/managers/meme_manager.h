/**
 * @file meme_manager.h
 * @brief Meme soundboard management
 * @details Handles meme file scanning and playback
 */

#pragma once

#include "Arduino.h"
#include <vector>

// External meme files list
extern std::vector<String> memeFiles;

/**
 * @brief Scan SD card for meme files
 */
void scanMemeFiles();

/**
 * @brief Get list of available meme files
 * @return Vector of meme file paths
 */
const std::vector<String>& getMemeFiles();

/**
 * @brief Play a specific meme file by index
 * @param index Meme file index (1-based)
 * @return true if meme was played successfully
 */
bool playMeme(int index);

/**
 * @brief Get meme files count
 * @return Number of available meme files
 */
size_t getMemeCount();
