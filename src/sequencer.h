#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include <vector>
#include <string>

// Structure to represent a note event
struct NoteEvent {
    String filename; // Path to the sample file (e.g., MP3)
    uint32_t startTimeMs; // When to play (ms from sequence start)
    uint32_t durationMs;  // How long to play (ms)
};

// Generates a sequence of NoteEvents for generative ambient music
// Parameters:
//   scaleFiles: vector of sample filenames representing the scale
//   numNotes: how many notes to generate
//   minDurationMs, maxDurationMs: range for note durations
//   minGapMs, maxGapMs: range for gaps between notes
// Returns: vector of NoteEvents
std::vector<NoteEvent> generateSequence(const std::vector<String>& scaleFiles, int numNotes, uint32_t minDurationMs, uint32_t maxDurationMs, uint32_t minGapMs, uint32_t maxGapMs);

#endif // SEQUENCER_H
