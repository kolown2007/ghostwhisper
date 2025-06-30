#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include "sequencer.h"
#include "secrets.h"
#include "musicdata.h"

#define I2S_DOUT     25
#define I2S_BCLK     27
#define I2S_LRC      26
Audio audio;

unsigned long lastSoundTime = 0;
int soundCounter = 0;
bool inAmbientMode = true;


std::vector<NoteEvent> sequence;
size_t currentNote = 0;
unsigned long sequenceStartTime = 0;
unsigned long lastNotePlayTime = 0;

void setup() {
   Serial.begin(9600);
  randomSeed(analogRead(A0)); // Seed random generator
  
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_ssid, WIFI_password);
  while (WiFi.status() != WL_CONNECTED)
    delay(1500);

  //setup audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(80); // audio volume

  //bell sounds setup - moved to loop for better control
  Serial.println("Setup complete, starting sequence in loop...");

}

void loop() {
  audio.loop();

  // Initialize sequence on first run
  static bool sequenceInitialized = false;
  if (!sequenceInitialized) {
    const auto& bellSounds = getBellSounds();
    size_t bellSounds_COUNT = getBellSoundsCount();
    
    // Print first few URLs to debug
    Serial.println("Sample bell sound URLs:");
    for (size_t i = 0; i < min(5, (int)bellSounds_COUNT); ++i) {
        Serial.println(String(i) + ": " + bellSounds[i]);
    }
    
    // Create URL vector for sequence generation - filter out invalid notes
    std::vector<String> urlVec;
    for (size_t i = 0; i < bellSounds_COUNT; ++i) {
        String url = bellSounds[i];
        // Skip problematic notes (Cb, Fb, very low notes)
        if (url.indexOf("Cb") == -1 && url.indexOf("Fb") == -1 && 
            url.indexOf("A0") == -1 && url.indexOf("B0") == -1) {
            urlVec.push_back(url);
        }
    }
    
    Serial.println("Filtered to " + String(urlVec.size()) + " valid URLs");    // Generate the sequence with more notes for longer sequences
    sequence = generateSequence(urlVec, 24, 2000, 4000, 1000, 3000); // 24 notes for extended ambient sequences
    sequenceStartTime = millis();
    sequenceInitialized = true;
    
    Serial.println("Sequence initialized with " + String(sequence.size()) + " notes");
  }
  // Play sequence with tick sound prevention
  unsigned long now = millis();
  if (currentNote < sequence.size()) {
    NoteEvent& evt = sequence[currentNote];    if (now - sequenceStartTime >= evt.startTimeMs) {
        // Prevent tick sounds between notes - longer delay
        delay(100); // Increased from 50ms to 100ms
        
        audio.connecttohost(evt.filename.c_str());
        lastNotePlayTime = now;
        currentNote++;
        
        Serial.println("Playing note " + String(currentNote) + "/" + String(sequence.size()) + ": " + evt.filename);
    }
  } else {
    // Sequence finished - regenerate for endless play
    Serial.println("Sequence complete, generating new sequence...");
    currentNote = 0;
    sequenceStartTime = millis();
    
    // Optionally regenerate with different parameters for variety
    const auto& bellSounds = getBellSounds();
    size_t bellSounds_COUNT = getBellSoundsCount();
    std::vector<String> urlVec;
    for (size_t i = 0; i < bellSounds_COUNT; ++i) {
        urlVec.push_back(String(bellSounds[i]));
    }
    sequence = generateSequence(urlVec, 24, 2000, 4000, 1000, 3000); // Match the note count at 24
  }
}