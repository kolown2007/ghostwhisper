/**
 * @file meme_manager.cpp
 * @brief Meme soundboard management implementation
 */

#include "meme_manager.h"
#include "../hardware/hardware_setup.h"
#include <SD.h>

std::vector<String> memeFiles;

void scanMemeFiles() {
    memeFiles.clear();
    Serial.println("Scanning for meme files...");
    File dir = SD.open("/meme");
    if (!dir) {
        Serial.println("/meme folder not found on SD card");
        Serial.println("Make sure you have a 'meme' folder at the root of your SD card");
        return;
    }
    Serial.println("Found /meme folder, scanning for .mp3 files...");
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;
        if (!entry.isDirectory()) {
            String fname = String(entry.name());
            if (fname.endsWith(".mp3")) {
                memeFiles.push_back("/meme/" + fname);
            }
        }
        entry.close();
    }
    dir.close();
    Serial.printf("Found %d meme files\n", memeFiles.size());
}

const std::vector<String>& getMemeFiles() {
    return memeFiles;
}

bool playMeme(int index) {
    if (index < 1 || index > memeFiles.size()) {
        Serial.println("Invalid meme index: " + String(index));
        return false;
    }
    
    String memePath = memeFiles[index - 1];
    Serial.println("Playing meme: " + memePath);
    
    audio.stopSong();
    audio.connecttoFS(SD, memePath.c_str());
    
    return true;
}

size_t getMemeCount() {
    return memeFiles.size();
}
