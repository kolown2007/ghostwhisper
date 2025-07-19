#pragma once


#include <SD.h>

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

// Music from server - using static to avoid multiple definition
static const char* musicFromServer[] = {
"https://kolown.net/storage/projects/whisper/Dayang%20Dayang.mp3"
};

// SD Card Music Files - using static to avoid multiple definition
static const std::vector<String> sdCardMusic = {
    "/music/dayang.mp3",
     "/music/La_Decadanse.mp3",
   
};

inline const std::vector<String>& getSDCardMusic() {
    return sdCardMusic;
}

inline size_t getSDCardMusicCount() {
    return sdCardMusic.size();
}
