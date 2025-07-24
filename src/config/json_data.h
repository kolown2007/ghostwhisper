#pragma once
#include <vector>
#include <Arduino.h>

// Reads a section from data.json and returns its file list
std::vector<String> getDataFilesFromJSON(const String& sectionName, const String& subsection = "");

// Helper function to get available subsections for a given section
std::vector<String> getAvailableSubsections(const String& sectionName);

// Random selection functions
std::vector<String> getRandomSoundfontFiles();

