#pragma once

#include "note_generator.h"



 const std::vector<String> bellSounds = generateNoteUrls(
    "https://gleitz.github.io/midi-js-soundfonts/FatBoy/", "music_box-mp3/");


const std::vector<String>& getBellSounds() {
    return bellSounds;
}


size_t getBellSoundsCount() { 
    return bellSounds.size(); 
}



const char* musicFromServer[] = {
"https://kolown.net/storage/projects/whisper/Dayang%20Dayang.mp3"
};
