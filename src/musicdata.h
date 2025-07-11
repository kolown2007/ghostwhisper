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



// SD Card Music Files
const std::vector<String> sdCardMusic = {
    "/music/dayang.mp3",
     "/music/La_Decadanse.mp3",
   
};

const std::vector<String>& getSDCardMusic() {
    return sdCardMusic;
}

size_t getSDCardMusicCount() {
    return sdCardMusic.size();
}
