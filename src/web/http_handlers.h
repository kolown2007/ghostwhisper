/**
 * @file http_handlers.h
 * @brief HTTP request handlers for web server endpoints
 * @details Contains all HTTP request handler function declarations
 */

#pragma once

#include "Arduino.h"

// Basic page handlers
void handleRoot();
void handleMemePage();
void handleStatus();
void handleNotFound();
void handleStaticFile();

// Volume control handlers
void handleVolumeUp();
void handleVolumeDown();
void handleSetVolume();
void handleVolumeTest();

// Playback control handlers
void handleRandomPlay();
void handleStop();
void handlePause();
void handleResume();

// System handlers
void handleMemoryCheck();
void handleWiFiReset();
void handleWiFiConfig();

// Radio program handlers
void handleProgramShuffle();
void handleProgramGenerative();
void handleProgramStream();
void handleProgramNewStream();
void handleProgramStatus();

// Program-specific handlers
void handleShuffleNext();
void handleShuffleFolder();
void handleGenerativeSequence();
void handleGenerativeRegenerate();
void handleStreamConnect();
void handleStreamReset();

// Meme soundboard handlers
void handleMemeList();
void handleMemePlay();
