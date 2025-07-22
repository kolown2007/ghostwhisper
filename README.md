**GhostWhisper**

----

Sound Installation for QC Biennale 2025

## Key Features (2025 Update)

- **#### Audio Processing
- **Library**: ESP32-audioI2S (Schreibfähig/esphome)
- **Streaming**: Real-time HTTP/HTTPS streaming and local SD playback
- **Buffer Management**: Built-in buffering for seamless playback
- **Volume Control**: Web-based volume adjustment (0-100%)
- **Pause/Resume/Stop**: Full playback control with proper state trackingle WiFi Connection Management**: Supports ONLINE (connects to network) and OFFLINE (local AP) modes. If WiFi fails, device automatically starts a configuration portal (visit `ghostwhisper.local` or `192.168.4.1`).
- **mDNS Hostname**: Access the web control panel via `ghostwhisper.local` instead of IP addresses.
- **Web Control Panel**: Select program, adjust volume, regenerate generative sequences, and control playback (pause/resume/stop) from your browser.
- **Generative Sequence Regeneration**: Instantly create a new generative melody via the web UI.
- **Pause/Resume/Stop Logic**: Fully supported via web UI and backend state tracking (ESP32-audioI2S library).
- **Error Handling**: Web UI displays clear error messages (e.g., stream mode only available in ONLINE mode).
- **Hardware Pins**: I2S and SD card pin assignments remain unchanged.

## Usage

1. **Power On**: ESP32 starts in configured mode (ONLINE/OFFLINE)
2. **WiFi Connection**: 
   - ONLINE mode: Connects to network or starts config portal
   - OFFLINE mode: Creates local AP at `192.168.4.1`
3. **Web Access**: Navigate to `ghostwhisper.local` (or IP address)
4. **Program Selection**: Choose SHUFFLE, GENERATIVE, or STREAM mode
5. **Playback Control**: Use web UI for play, pause, resume, stop, volume
6. **Sequence Regeneration**: Click "Regenerate Sequence" for new generative melodies
7. **WiFi Management**: Use `/wifi/reset` or `/wifi/config` routes as needed

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
- **WiFi Connectivity** - For streaming audio from remote sources and web control interface

### Software Components

#### Core Architecture (Modular Design)

**`main.cpp`** - *Application Entry Point*
- High-level application flow orchestration
- Connection mode selection (ONLINE/OFFLINE)
- Hardware initialization and radio program management

**Connection Management:**
- **`connection_manager.h/.cpp`** - WiFi connection handling
  - ONLINE/OFFLINE mode support
  - WiFiManager configuration portal
  - mDNS hostname setup (`ghostwhisper.local`)
  - LED status indicators

**Hardware Layer:**
- **`hardware_setup.h/.cpp`** - Hardware initialization and configuration
  - I2S audio interface setup (ESP32-audioI2S)
  - SD card initialization
  - Pin definitions and hardware abstraction
- **`config.h`** - System configuration constants
  - WiFi settings, timeouts, pin definitions

**Program Management:**
- **`radio_manager.h/.cpp`** - Main program coordination
  - Handles shuffle, generative, and stream programs
  - Program state management and transitions
  
- **`shuffle_manager.h/.cpp`** - Random audio file playback
  - SD card file scanning and random selection
  - Folder-based music organization
  
- **`generative_manager.h/.cpp`** - Algorithmic music generation
  - Harmonious sequence generation from soundfont files
  - Markov-chain style note progression
  - On-demand sequence regeneration
  
- **`stream_manager.h/.cpp`** - Online audio streaming
  - Remote URL streaming (requires ONLINE mode)
  - HTTP/HTTPS audio stream handling

**Control Interface:**
- **`control.h/.cpp`** - Web server and HTTP endpoints
  - Volume control, program switching, playback control
  - WiFi management routes (/wifi/reset, /wifi/config)
  - Pause/resume/stop with proper state tracking
  - Status reporting and system monitoring

**Utilities:**
- **`debug_manager.h/.cpp`** - Logging and diagnostics
  - Audio status monitoring
  - Error detection and reporting
  - Debug output management

**Data Layer:**
- **`musicdata.h`** - Sound library definitions
  - Soundfont file references for generative mode
  - Access functions `getSoundfontFiles()` and `getSoundfontFilesCount()`
  - Local SD card audio file management

**Configuration:**
- **`secrets.h`** - Sensitive configuration (not in repo)
  - WiFi credentials and server endpoints
  - Not committed to version control for security

### Audio System

#### Sound Sources
- **Local SD Card**: WAV files for shuffle mode (random playback)
- **Generative Soundfont**: Local soundfont samples for algorithmic composition
- **Remote Streams**: Online audio streams (ONLINE mode only)
- **Format**: WAV audio files (ESP32-audioI2S library)

#### Audio Processing
- **Library**: Arduino Audio Tools (replaced ESP32-audioI2S)
- **Streaming**: Real-time HTTP/HTTPS streaming and local SD playback
- **Buffer Management**: StreamCopy for WAV playback
- **Volume Control**: Web-based volume adjustment (0-100%)
- **Pause/Resume/Stop**: Full playback control with proper state tracking

### Generative Algorithm

#### Harmonious Sequence Generation
The generative manager creates musically coherent sequences using local soundfont files:

```cpp
// Creates sequences with 3-12 notes from available soundfont files
// Uses random selection with timing intervals of 5-50 seconds
// Generates new sequence when current one completes
```

#### Program Modes
- **SHUFFLE**: Random playback of SD card audio files
- **GENERATIVE**: Algorithmic composition using soundfont samples  
- **STREAM**: Online audio stream playback (ONLINE mode required)

#### Sequence Parameters
- **Sequence Length**: 3-12 notes per cycle (dynamic)
- **Note Duration**: 5-50 seconds per note (random intervals)
- **Regeneration**: On-demand via web UI "Regenerate Sequence" button
- **Source Files**: Local soundfont samples from SD card

## Configuration

### WiFi Setup
Configure in `secrets.h`:
```cpp
const char* WIFI_ssid = "your_network";
const char* WIFI_password = "your_password";
```

**WiFiManager Portal:**
- If WiFi connection fails, device automatically starts a configuration portal
- Access via `ghostwhisper.local` or `192.168.4.1`
- Web UI provides `/wifi/reset` and `/wifi/config` routes

### Audio Settings
```cpp
audio.setVolume(80);           // Volume level (0-100)
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); // I2S pin configuration
```

### Connection Modes
Configure in `main.cpp`:
```cpp
initializeConnection(OFFLINE); // Local AP mode
// or
initializeConnection(ONLINE);  // Connect to WiFi network
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
- **Dependencies**: 
  - ESP32-audioI2S library
  - WiFiManager for configuration portal
  - WiFi, SD, and WebServer libraries for ESP32
- **Configuration**: `platformio.ini`
- **Architecture**: Modular design with manager pattern

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

1. **Power On**: ESP32 starts in configured mode (ONLINE/OFFLINE)
2. **WiFi Connection**: 
   - ONLINE mode: Connects to network or starts config portal
   - OFFLINE mode: Creates local AP at `192.168.4.1`
3. **Web Access**: Navigate to `ghostwhisper.local` (or IP address)
4. **Program Selection**: Choose SHUFFLE, GENERATIVE, or STREAM mode
5. **Playback Control**: Use web UI for play, pause, resume, stop, volume
6. **Sequence Regeneration**: Click "Regenerate Sequence" for new generative melodies
7. **WiFi Management**: Use `/wifi/reset` or `/wifi/config` routes as needed

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
┌─────────▼─────────┐
│ connection_manager│ ← WiFi & mDNS Setup
└─────────┬─────────┘
          │
┌─────────▼─────────┐
│ hardware_setup    │ ← I2S Audio & SD Card
└─────────┬─────────┘
          │
┌─────────▼─────────┐
│  radio_manager    │ ← Program Coordination
└─────────┬─────────┘
          │
┌─────────▼─────────┐
│Program Managers   │ ← shuffle/generative/stream
└───────────────────┘
```

### Detailed Dependency Map
```
main.cpp
├── connection_manager.h/.cpp
│   ├── WiFiManager (configuration portal)
│   ├── ESPmDNS (hostname resolution)
│   └── config.h (WiFi settings)
│
├── hardware_setup.h/.cpp
│   ├── Audio (Arduino Audio Tools)
│   ├── SD (card interface)
│   └── secrets.h (WiFi credentials)
│
├── radio_manager.h/.cpp
│   ├── shuffle_manager.h/.cpp
│   ├── generative_manager.h/.cpp
│   ├── stream_manager.h/.cpp
│   └── hardware_setup.h (audio object)
│
├── control.h/.cpp (Web Server)
│   ├── WebServer (HTTP endpoints)
│   ├── connection_manager.h (WiFi status)
│   └── All program managers
│
└── debug_manager.h/.cpp
    └── Serial logging functions
```

### Data Flow Process
```
1. INITIALIZATION
   main.cpp → connection_manager → WiFi/mDNS setup
      ↓
   hardware_setup → Audio/SD initialization
      ↓
   radio_manager → Program state setup
      ↓
   control → Web server start

2. PROGRAM SELECTION (via Web UI)
   Web UI → control.cpp → radio_manager
      ↓
   radio_manager → [shuffle/generative/stream]_manager
      ↓
   Program manager → hardware_setup (audio object)

3. GENERATIVE PLAYBACK CYCLE
   generative_manager → musicdata.h (soundfont files)
      ↓
   Generate harmonious sequence
      ↓
   Play sequence with timing intervals
      ↓
   On regeneration → clear sequence → repeat

4. WEB CONTROL
   Browser → ghostwhisper.local → control.cpp
      ↓
   HTTP endpoints → Program managers
      ↓
   Status updates → JSON responses
```

### Inter-Module Communication
- **main.cpp** orchestrates high-level initialization flow
- **connection_manager** handles WiFi modes and mDNS setup
- **radio_manager** coordinates program switching and state management  
- **control.cpp** provides web interface and HTTP API endpoints
- **[program]_managers** handle specific audio playback logic
- **hardware_setup** provides shared audio object and SD access
- **debug_manager** monitors operations across all modules