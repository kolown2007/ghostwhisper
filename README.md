# GhostWhisper
![GhostWhisper Logo](assets/ghostwhisper.png)

mp3 player + generative sequencer + internet radio player

developed by KoloWn and Roan Alvarez for QCB 2025

**Disclaimer**: GhostWhisper is a platform for artists to present their own sound files and generative content. All audio and materials used are the sole responsibility and artistic expression of each artist. The platform itself does not claim ownership or legal responsibility for any content; future artists using GhostWhisper are fully responsible for the legality and originality of their works.

**Note**: This project is under active development. A detailed roadmap and feature plan will be published soon.

**Project Structure**
- src - c++ / arduino flavor on esp32 platform io 
- scripts - python / automation scripts
- view - html/css/javascript
- assets - png/svg

## Program 

- **SHUFFLE**: random audio playback
- **GENERATIVE**: Algorithmic soundfont composition
- **STREAM**: ONLINE mode only and under development

## Modes

- **ONLINE**: 
- **OFFLINE**: 

for QCB, the default mode is OFFLINE and GENERATIVE program 

## Access

- **URL**: `ghostwhisper.local` 
- **Controls**: Volume, program selection, playback control via web UI


## Hardware Pins

**I2S Audio**: DOUT=25, BCLK=27, LRC=26  
**SD Card**: MOSI=23, MISO=19, SCK=18, CS=5

## SD card folder structure
- fields: any mp3 file
- meme: any mp3 file
- music : any mp3 files
- soundfont: with subfolders that stores your soundfonts
- view: the web-controller
- data.json: generate this file using the scan_sd_card.py