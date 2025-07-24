#pragma once

#include <vector>
#include <SD.h>

static const char* MUSIC_NOTES[] = {
    "A0.mp3", "A1.mp3", "A2.mp3", "A3.mp3", "A4.mp3", "A5.mp3", "A6.mp3", "A7.mp3",
    "Ab1.mp3", "Ab2.mp3", "Ab3.mp3", "Ab4.mp3", "Ab5.mp3", "Ab6.mp3", "Ab7.mp3",
    "B0.mp3", "B1.mp3", "B2.mp3", "B3.mp3", "B4.mp3", "B5.mp3", "B6.mp3", "B7.mp3",
    "Bb0.mp3", "Bb1.mp3", "Bb2.mp3", "Bb3.mp3", "Bb4.mp3", "Bb5.mp3", "Bb6.mp3", "Bb7.mp3",
    "C1.mp3", "C2.mp3", "C3.mp3", "C4.mp3", "C5.mp3", "C6.mp3", "C7.mp3",
    "D1.mp3", "D2.mp3", "D3.mp3", "D4.mp3", "D5.mp3", "D6.mp3", "D7.mp3",
    "Db1.mp3", "Db2.mp3", "Db3.mp3", "Db4.mp3", "Db5.mp3", "Db6.mp3", "Db7.mp3",
    "E1.mp3", "E2.mp3", "E3.mp3", "E4.mp3", "E5.mp3", "E6.mp3", "E7.mp3",
    "Eb1.mp3", "Eb2.mp3", "Eb3.mp3", "Eb4.mp3", "Eb5.mp3", "Eb6.mp3", "Eb7.mp3",
    "F1.mp3", "F2.mp3", "F3.mp3", "F4.mp3", "F5.mp3", "F6.mp3", "F7.mp3",
    "G1.mp3", "G2.mp3", "G3.mp3", "G4.mp3", "G5.mp3", "G6.mp3", "G7.mp3",
    "Gb1.mp3", "Gb2.mp3", "Gb3.mp3", "Gb4.mp3", "Gb5.mp3", "Gb6.mp3", "Gb7.mp3"
};
static const size_t MUSIC_NOTES_COUNT = sizeof(MUSIC_NOTES) / sizeof(MUSIC_NOTES[0]);




// Stream configuration
struct StreamConfig {
    String name;
    String url;
};

// Available radio streams - using static to avoid multiple definition
static const std::vector<StreamConfig> availableStreams = {
    {"Smooth Jazz", "http://jazz-wr04.ice.infomaniak.ch/jazz-wr04-128.mp3"},
    {"Reggae", "http://reggae.stream.laut.fm/reggae"},
   
};

// NEW: Force new stream URL to bypass any cached state
static const String FORCE_NEW_STREAM_URL = "https://reggae.stream.laut.fm/reggae";

// Get default stream URL
inline const String& getDefaultStreamURL() {
    static String defaultURL = availableStreams[0].url; // Reggae Radio
    return defaultURL;
}

// NEW: Get forced new stream URL
inline const String& getForceNewStreamURL() {
    return FORCE_NEW_STREAM_URL;
}

// Get stream URL by name
inline String getStreamURLByName(const String& name) {
    for (const auto& stream : availableStreams) {
        if (stream.name.equalsIgnoreCase(name)) {
            return stream.url;
        }
    }
    return getDefaultStreamURL(); // Return default if not found
}

// Function to get soundfont files from SD card
inline std::vector<String> getSoundfontFilesFromSD() {
    std::vector<String> sounds;
    const char* baseFolder = "/soundfont";

    File baseDir = SD.open(baseFolder);
    if (!baseDir || !baseDir.isDirectory()) {
        Serial.println("ERROR: Unable to open SD base folder: " + String(baseFolder));
        return sounds;
    }

    // Collect subfolders
    std::vector<String> subfolders;
    File subDir = baseDir.openNextFile();
    while (subDir) {
        if (subDir.isDirectory()) {
            subfolders.push_back(String(baseFolder) + "/" + subDir.name());
        }
        subDir.close();
        subDir = baseDir.openNextFile();
    }
    baseDir.close();

    if (subfolders.empty()) {
        Serial.println("ERROR: No subfolders found in " + String(baseFolder));
        return sounds;
    }

    // Select a random subfolder
    size_t randomIndex = random(0, subfolders.size());
    const String& selectedFolder = subfolders[randomIndex];

    File dir = SD.open(selectedFolder.c_str());
    if (!dir || !dir.isDirectory()) {
        Serial.println("ERROR: Unable to open SD folder: " + selectedFolder);
        return sounds;
    }

    File entry = dir.openNextFile();
    while (entry) {
        String name = entry.name();
        if (name.endsWith(".mp3")) {
            sounds.push_back(selectedFolder + "/" + name);
        }
        entry.close();
        entry = dir.openNextFile();
    }
    dir.close();

    Serial.println("Found " + String(sounds.size()) + " soundfont files in " + selectedFolder);
    return sounds;
}

inline const std::vector<String>& getSoundfontFiles() {
    static std::vector<String> soundfontFiles = getSoundfontFilesFromSD();
    return soundfontFiles;
}

inline size_t getSoundfontFilesCount() { 
    return getSoundfontFiles().size(); 
}

