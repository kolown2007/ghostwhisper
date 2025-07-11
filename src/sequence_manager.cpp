#include "sequence_manager.h"
#include "musicdata.h"
#include "url_filter.h"

// Static variables for sequence management
static std::vector<NoteEvent> sequence;
static size_t currentNote = 0;
static unsigned long sequenceStartTime = 0;
static bool sequenceInitialized = false;

void initializeSequence() {
    // Simplified: Use SD card music instead of complex sequencing
    const auto& sdMusic = getSDCardMusic();
    size_t sdMusicCount = getSDCardMusicCount();
    
    Serial.println("Using SD card music files:");
    for (size_t i = 0; i < sdMusicCount; ++i) {
        Serial.println(String(i) + ": " + sdMusic[i]);
    }
    
    // Create simple sequence with SD files (no filtering needed)
    std::vector<String> sdFiles;
    for (size_t i = 0; i < sdMusicCount; ++i) {
        sdFiles.push_back(sdMusic[i]);
    }
    
    // Cache SD files for future use
    cacheValidUrls(sdFiles);
    
    // Generate simple sequence with SD files
    sequence = generateSequence(sdFiles, 3, 5000, 10000, 2000, 5000); // 3 files, longer durations for songs
    sequenceStartTime = millis();
    sequenceInitialized = true;
    
    Serial.println("SD sequence initialized with " + String(sequence.size()) + " files");
    
    /* COMMENTED OUT - Web streaming sequence generation
    const auto& bellSounds = getBellSounds();
    size_t bellSounds_COUNT = getBellSoundsCount();
    
    // Print first few URLs to debug
    Serial.println("Sample bell sound URLs:");
    for (size_t i = 0; i < min(5, (int)bellSounds_COUNT); ++i) {
        Serial.println(String(i) + ": " + bellSounds[i]);
    }
    
    Serial.println("Creating sequence with filtered URLs...");
    
    // Get filtered URLs
    std::vector<String> urlVec = getFilteredUrls(bellSounds.data(), bellSounds_COUNT);
    
    Serial.println("Using " + String(urlVec.size()) + " filtered URLs out of " + String(bellSounds_COUNT));
    
    // Cache filtered URLs for future use
    cacheValidUrls(urlVec);
    
    // Generate the sequence with more notes for longer sequences
    sequence = generateSequence(urlVec, 24, 2000, 4000, 1000, 3000); // 24 notes for extended ambient sequences
    */
}

void updateSequence() {
    if (isSequenceComplete()) {
        regenerateSequence();
    }
}

bool isSequenceInitialized() {
    return sequenceInitialized;
}

bool isSequenceComplete() {
    return currentNote >= sequence.size();
}

void regenerateSequence() {
    // Sequence finished - regenerate for endless play
    Serial.println("Sequence complete, generating new sequence...");
    currentNote = 0;
    sequenceStartTime = millis();
    
    // For regeneration, use the cached filtered URLs (fast)
    std::vector<String> cachedUrls = getCachedUrls();
    Serial.println("Using " + String(cachedUrls.size()) + " pre-filtered URLs for new sequence");
    sequence = generateSequence(cachedUrls, 24, 2000, 4000, 1000, 3000);
}

size_t getCurrentNoteIndex() {
    return currentNote;
}

const NoteEvent& getCurrentNote() {
    return sequence[currentNote];
}

unsigned long getSequenceStartTime() {
    return sequenceStartTime;
}

void advanceToNextNote() {
    currentNote++;
}

size_t getSequenceSize() {
    return sequence.size();
}
