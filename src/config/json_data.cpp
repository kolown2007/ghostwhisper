// Reads a section from data.json and returns its file list
#include "json_data.h"
#include <vector>
#include <Arduino.h>
#include <SD.h>

std::vector<String> getDataFilesFromJSON(const String& sectionName, const String& subsection) {
    std::vector<String> files;
    File dataFile = SD.open("/data.json");
    if (!dataFile) {
        Serial.println("ERROR: Unable to open data.json");
        return files;
    }
    String jsonContent = "";
    while (dataFile.available()) {
        jsonContent += char(dataFile.read());
    }
    dataFile.close();

    // Find the main section
    String searchKey = "\"" + sectionName + "\"";
    int sectionStart = jsonContent.indexOf(searchKey);
    if (sectionStart == -1) {
        Serial.println("Section '" + sectionName + "' not found in data.json");
        return files;
    }

    // If subsection is specified, look for it
    int searchStart = sectionStart;
    String logSection = sectionName;
    
    if (subsection.length() > 0) {
        String subsectionKey = "\"" + subsection + "\"";
        int subsectionStart = jsonContent.indexOf(subsectionKey, sectionStart);
        if (subsectionStart == -1) {
            Serial.println("Subsection '" + subsection + "' not found in '" + sectionName + "'");
            return files;
        }
        searchStart = subsectionStart;
        logSection = sectionName + "/" + subsection;
    }

    // Find the "files" array
    int filesKeyStart = jsonContent.indexOf("\"files\"", searchStart);
    if (filesKeyStart == -1) {
        Serial.println("No 'files' array found for section: " + logSection);
        return files;
    }
    
    int filesArrayStart = jsonContent.indexOf('[', filesKeyStart);
    int filesArrayEnd = jsonContent.indexOf(']', filesArrayStart);
    if (filesArrayStart == -1 || filesArrayEnd == -1) {
        Serial.println("Invalid 'files' array for section: " + logSection);
        return files;
    }
    
    // Parse the files array
    String filesStr = jsonContent.substring(filesArrayStart + 1, filesArrayEnd);
    int start = 0;
    while (start < filesStr.length()) {
        int quoteStart = filesStr.indexOf('"', start);
        if (quoteStart == -1) break;
        int quoteEnd = filesStr.indexOf('"', quoteStart + 1);
        if (quoteEnd == -1) break;
        String fileName = filesStr.substring(quoteStart + 1, quoteEnd);
        files.push_back(fileName);
        start = quoteEnd + 1;
    }
    
    Serial.println("Loaded " + String(files.size()) + " files from section '" + logSection + "'");
    return files;
}

// Helper function to get available subsections for a given section
std::vector<String> getAvailableSubsections(const String& sectionName) {
    std::vector<String> subsections;
    File dataFile = SD.open("/data.json");
    if (!dataFile) return subsections;
    
    String jsonContent = "";
    while (dataFile.available()) {
        jsonContent += char(dataFile.read());
    }
    dataFile.close();

    // Find the main section
    String searchKey = "\"" + sectionName + "\"";
    int sectionStart = jsonContent.indexOf(searchKey);
    if (sectionStart == -1) return subsections;
    
    // Find the opening brace of the section
    int braceStart = jsonContent.indexOf('{', sectionStart);
    if (braceStart == -1) return subsections;
    
    // Find matching closing brace
    int braceCount = 1;
    int pos = braceStart + 1;
    int sectionEnd = -1;
    
    while (pos < jsonContent.length() && braceCount > 0) {
        if (jsonContent.charAt(pos) == '{') braceCount++;
        else if (jsonContent.charAt(pos) == '}') braceCount--;
        if (braceCount == 0) sectionEnd = pos;
        pos++;
    }
    
    if (sectionEnd == -1) return subsections;
    
    // Extract all keys that have a "files" array (indicating they're subsections)
    String sectionContent = jsonContent.substring(braceStart, sectionEnd);
    int searchPos = 0;
    
    while (searchPos < sectionContent.length()) {
        int keyStart = sectionContent.indexOf('"', searchPos);
        if (keyStart == -1) break;
        int keyEnd = sectionContent.indexOf('"', keyStart + 1);
        if (keyEnd == -1) break;
        
        String key = sectionContent.substring(keyStart + 1, keyEnd);
        
        // Check if this key has a "files" array
        int keyValueStart = sectionContent.indexOf(':', keyEnd);
        if (keyValueStart != -1) {
            int filesCheck = sectionContent.indexOf("\"files\"", keyValueStart);
            int nextKeyCheck = sectionContent.indexOf('"', keyValueStart + 1);
            
            // If "files" is found before the next key, this is a subsection
            if (filesCheck != -1 && (nextKeyCheck == -1 || filesCheck < nextKeyCheck)) {
                subsections.push_back(key);
            }
        }
        
        searchPos = keyEnd + 1;
    }
    
    return subsections;
}

// Random selection functions
std::vector<String> getRandomSoundfontFiles() {
    std::vector<String> subsections = getAvailableSubsections("soundfont");
    if (subsections.empty()) {
        Serial.println("No soundfont subsections found");
        return std::vector<String>();
    }
    
    int randomIndex = random(0, subsections.size());
    String randomSubsection = subsections[randomIndex];
    
    Serial.println("Randomly selected soundfont collection: " + randomSubsection);
    return getDataFilesFromJSON("soundfont", randomSubsection);
}
