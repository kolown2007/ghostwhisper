**GhostWhisper**

----

Internet Radio Project for QC Biennale 2025

## Overview
GhostWhisper is an ambient music generation system that creates real-time generative soundscapes using ESP32 hardware and online sound libraries. The system streams musical notes from remote servers and uses Markov chain algorithms to create evolving ambient compositions.

## System Architecture

### Hardware Components
- **ESP32 Development Board** - Main microcontroller
- **I2S Audio Output** - Digital audio interface
  - I2S_DOUT: Pin 25
  - I2S_BCLK: Pin 27 
  - I2S_LRC: Pin 26
- **SD Card Interface** - Local storage for audio files
  - MOSI: Pin 23
  - MISO: Pin 19
  - CLK: Pin 18
  - CS: Pin 5
- **WiFi Connectivity** - For streaming audio from remote sources

### Software Components

#### Core Architecture (Modular Design)

**`main.cpp`** - *Application Entry Point*
- High-level application flow orchestration
- Delegates to specialized manager modules
- Minimal, clean implementation focused on coordination

**Hardware Layer:**
- **`hardware_setup.h/.cpp`** - Hardware initialization and configuration
  - WiFi connection management
  - I2S audio interface setup
  - SD card initialization
  - Pin definitions and hardware abstraction

**Manager Layer:**
- **`sequence_manager.h/.cpp`** - Sequence lifecycle management
  - Sequence initialization and regeneration
  - Current note tracking and progression
  - Sequence completion detection
  
- **`playback_manager.h/.cpp`** - Audio playback coordination
  - Transition timing and gap management
  - Audio streaming control
  - Note advancement logic
  
- **`debug_manager.h/.cpp`** - Logging and diagnostics
  - Audio status monitoring
  - Error detection and reporting
  - Debug output management
  
- **`url_filter.h/.cpp`** - URL processing and caching
  - Problematic URL filtering
  - Validated URL caching
  - Filter criteria management

**Data Layer:**
- **`musicdata.h`** - Sound library definitions
  - Bell sounds URL collection
  - Access functions `getBellSounds()` and `getBellSoundsCount()`
  - External soundfont library connections

**Algorithm Layer:**
- **`note_generator.h/.cpp`** - Musical note URL generation
  - Note patterns from A0 to Gb7
  - URL construction for soundfont servers
  - Functions: `generateNoteUrls()`, `getNoteUrl()`

- **`sequencer.h/.cpp`** - Generative music composition
  - Markov chain algorithms for musical progression
  - Timing and duration management
  - Key structures: `NoteEvent`, `generateSequence()`

**Configuration:**
- **`secrets.h`** - Sensitive configuration
  - WiFi credentials and server endpoints
  - Not committed to version control for security

### Audio System

#### Sound Sources
- **Remote Soundfonts**: Streams from `gleitz.github.io/midi-js-soundfonts`
- **Instrument**: Music box samples (warm, ambient character)
- **Format**: MP3 audio files at 44.1kHz, 128kbps

#### Audio Processing
- **Streaming**: Real-time HTTP/HTTPS streaming
- **Buffer Management**: Automatic audio buffer handling
- **Gap Control**: 200ms gaps between notes to prevent audio artifacts
- **Volume**: Configurable output level (default: 80%)

### Generative Algorithm

#### Markov Chain Implementation
The sequencer uses Markov chains to create musically coherent progressions:

```cpp
// Example transition probabilities
{0, {{1, 0.4f}, {2, 0.6f}}}, // From note 0: 40% to note 1, 60% to note 2
{1, {{0, 0.5f}, {2, 0.5f}}}, // From note 1: 50% to note 0, 50% to note 2
```

#### Sequence Parameters
- **Sequence Length**: 24 notes per cycle
- **Note Duration**: 2-4 seconds per note
- **Gap Between Notes**: 1-3 seconds
- **Total Cycle Time**: ~72-168 seconds (1.2-2.8 minutes)

#### URL Filtering
Basic filtering removes problematic note combinations:
- Excludes non-existent notes (Cb, Fb)
- Filters out very low octaves (octave 0)
- Prevents 404 errors that cause audio artifacts

## Configuration

### WiFi Setup
Configure in `secrets.h`:
```cpp
const char* WIFI_ssid = "your_network";
const char* WIFI_password = "your_password";
```

### Audio Settings
```cpp
audio.setVolume(80);           // Volume level (0-100)
audio.setPinout(BCLK, LRC, DOUT); // I2S pin configuration
```

### Sequence Tuning
Modify in `sequence_manager.cpp`:
```cpp
generateSequence(urlVec, 24, 2000, 4000, 1000, 3000);
//               notes^  duration^   gaps^
```

### Hardware Pin Configuration
Modify in `hardware_setup.cpp`:
```cpp
// I2S Audio Pins
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

// SD Card SPI Pins  
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18
#define SD_CS 5
```

## Development

### Build System
- **Platform**: PlatformIO with ESP32 framework
- **Dependencies**: WiFi, Audio, SD libraries for ESP32
- **Configuration**: `platformio.ini`
- **Architecture**: Modular design with separation of concerns

### Code Organization
- **Manager Pattern**: Specialized modules for different responsibilities
- **Header/Implementation Split**: Standard C++ practice
- **Clean Architecture**: Main function orchestrates, managers implement
- **Dependency Injection**: Modules communicate through well-defined interfaces

### Debugging
- **Serial Monitor**: Real-time logging at 115200 baud
- **Modular Logging**: Each manager provides specific debug information
- **Audio Callbacks**: Stream status and error reporting via debug_manager
- **Timing Logs**: Sequence progression monitoring

### Memory Management
- **Dynamic Vectors**: For flexible URL storage
- **Reference Passing**: Minimizes memory copying
- **Caching**: Reuses filtered URL collections

## Usage

1. **Power On**: ESP32 connects to WiFi automatically
2. **Initialization**: System loads and filters note URLs
3. **Playback**: Begins ambient sequence generation
4. **Continuous Operation**: Generates new sequences when current ones complete

## Troubleshooting

### Common Issues
- **Audio Ticks**: Usually caused by 404 errors from missing sound files
- **WiFi Connection**: Check credentials in `secrets.h`
- **Memory Issues**: Monitor heap usage in Serial output
- **Timing Problems**: Verify I2S pin connections

### Serial Monitor Output
Watch for:
- `Setup complete, starting sequence in loop...` - System initialization
- `Sequence initialized with X notes` - Successful sequence creation  
- `Starting transition gap for next note` - Note transitions
- `Gap complete, playing: [URL]` - Audio playback start
- `Audio info:` - Stream status updates
- `WARNING: 404 Error detected` - Missing sound files

## Data Flow and Dependencies

### System Flow Diagram
```
┌─────────────────┐
│    main.cpp     │ ← Application Entry Point
└─────────┬───────┘
          │
    ┌─────▼─────┐
    │ Managers  │ ← Coordination Layer
    └─────┬─────┘
          │
┌─────────▼─────────┐
│   Data Sources    │ ← Information Layer
└───────────────────┘
```

### Detailed Dependency Map
```
main.cpp
├── hardware_setup.h/.cpp
│   ├── secrets.h (WiFi credentials)
│   └── Audio (external library)
│
├── sequence_manager.h/.cpp
│   ├── url_filter.h/.cpp
│   │   └── (filters URLs from musicdata)
│   ├── musicdata.h (bell sounds data)
│   └── sequencer.h/.cpp (Markov chains)
│
├── playback_manager.h/.cpp
│   ├── hardware_setup.h (audio object)
│   └── sequence_manager.h (note progression)
│
└── debug_manager.h/.cpp
    └── (audio callback functions)
```

### Data Flow Process
```
1. INITIALIZATION
   main.cpp → hardware_setup → WiFi/Audio/SD Card

2. SEQUENCE CREATION
   main.cpp → sequence_manager → url_filter → musicdata.h
                              ↓
                           sequencer.h (Markov chains)
                              ↓
                         Generated sequence

3. PLAYBACK CYCLE
   main.cpp → playback_manager → audio.connecttohost()
      ↓           ↓
   sequence_manager (advance note)
      ↓
   debug_manager (logging)

4. REGENERATION
   sequence complete → sequence_manager → (repeat from step 2)
```

### Inter-Module Communication
- **main.cpp** orchestrates high-level flow
- **sequence_manager** provides current note state  
- **playback_manager** handles audio timing
- **url_filter** preprocesses data for sequence_manager
- **debug_manager** monitors all operations
- **hardware_setup** provides shared audio object