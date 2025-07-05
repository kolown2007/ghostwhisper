#include "note_generator.h"

// Generate URLs for all notes from A0 to Gb7 in the specified pattern
std::vector<String> generateNoteUrls(const String& baseUrl, const String& instrument) {
    std::vector<String> urls;
      // Define valid musical notes (removed Cb and Fb which don't exist)
    const char* noteNames[] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
    const int numNoteNames = sizeof(noteNames) / sizeof(noteNames[0]);
    
    // For each note name, generate the octave sequence
    for (int i = 0; i < numNoteNames; i++) {
        const char* noteName = noteNames[i];
        
        // Determine start and end octaves
        int startOctave = 0;
        int endOctave = 7;
        
        // If we're at the last note (Gb) only go up to Gb7
        if (strcmp(noteName, "Gb") == 0) {
            // We only go up to Gb7 and then stop
            endOctave = 7;
        }
        
        // Generate URLs for each octave of this note
        for (int octave = startOctave; octave <= endOctave; octave++) {
            // Some notes may not exist in certain octaves on a piano
            // For example, there's no A8 on a standard 88-key piano
            // You may want to add additional checks here
            
            // Create the URL for this note
            String noteUrl = getNoteUrl(baseUrl, instrument, String(noteName), octave);
            urls.push_back(noteUrl);
            
            // If this is Gb7, we've reached the end of the sequence
            if (strcmp(noteName, "Gb") == 0 && octave == 7) {
                break;
            }
        }
    }
    
    return urls;
}

// Helper function to get a specific note URL
String getNoteUrl(const String& baseUrl, const String& instrument, const String& note, int octave) {
    // Remove trailing slash from baseUrl if it exists to prevent double slashes
    String cleanBaseUrl = baseUrl;
    if (cleanBaseUrl.endsWith("/")) {
        cleanBaseUrl = cleanBaseUrl.substring(0, cleanBaseUrl.length() - 1);
    }
    return cleanBaseUrl + "/" + instrument + note + String(octave) + ".mp3";
}
