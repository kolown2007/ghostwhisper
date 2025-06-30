#ifndef NOTE_GENERATOR_H
#define NOTE_GENERATOR_H

#include <Arduino.h>
#include <vector>
#include <string>

// Generates URLs for notes in the specified pattern from A0 to Gb7
std::vector<String> generateNoteUrls(const String& baseUrl, const String& instrument);

// Helper function to get a specific note URL
String getNoteUrl(const String& baseUrl, const String& instrument, const String& note, int octave);

#endif // NOTE_GENERATOR_H
