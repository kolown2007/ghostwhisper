#include "sequencer.h"
#include <map>
#include <vector>
#include <utility>

// Markov transition table: each note index maps to a vector of (next note index, probability)
using MarkovTable = std::map<int, std::vector<std::pair<int, float>>>;

// Helper: select next note index based on Markov probabilities
int selectMarkovNext(int currentIdx, const MarkovTable& table) {
    auto it = table.find(currentIdx);
    if (it == table.end() || it->second.empty()) {
        // Fallback: random if no transitions defined
        return random(0, table.size());
    }
    float r = (float)random(0, 10000) / 10000.0f;
    float cumulative = 0.0f;
    for (const auto& pair : it->second) {
        cumulative += pair.second;
        if (r < cumulative) {
            return pair.first;
        }
    }
    // Fallback: return last if rounding error
    return it->second.back().first;
}

std::vector<NoteEvent> generateSequence(const std::vector<String>& scaleFiles, int numNotes, uint32_t minDurationMs, uint32_t maxDurationMs, uint32_t minGapMs, uint32_t maxGapMs) {
    std::vector<NoteEvent> sequence;
    uint32_t currentTime = 0;
    // Example Markov table for 5 notes (adjust as needed)
    MarkovTable markov = {
        {0, {{1, 0.4f}, {2, 0.6f}}}, // from note 0, 40% to 1, 60% to 2
        {1, {{0, 0.5f}, {2, 0.5f}}},
        {2, {{1, 0.7f}, {3, 0.3f}}},
        {3, {{4, 1.0f}}},
        {4, {{0, 1.0f}}}
    };
    int noteIdx = random(0, scaleFiles.size()); // Start with a random note
    for (int i = 0; i < numNotes; ++i) {
        uint32_t duration = random(minDurationMs, maxDurationMs + 1);
        NoteEvent evt;
        evt.filename = scaleFiles[noteIdx];
        evt.startTimeMs = currentTime;
        evt.durationMs = duration;
        sequence.push_back(evt);
        uint32_t gap = random(minGapMs, maxGapMs + 1);
        currentTime += duration + gap;
        noteIdx = selectMarkovNext(noteIdx, markov);
    }
    return sequence;
}
