<?php

// $jsonUrl = 'https://gleitz.github.io/midi-js-soundfonts/FatBoy/names.json';

// Base URL of the soundfont .mp3 files
// $baseUrl   = 'https://gleitz.github.io/midi-js-soundfonts/FatBoy/music_box-mp3/';
$baseUrl   = 'https://gleitz.github.io/midi-js-soundfonts/FatBoy/fx_8_scifi-mp3/';
$saveDir   = __DIR__ . '/soundfonts/';

// 1. Make sure the save directory exists
if (!is_dir($saveDir)) {
    mkdir($saveDir, 0755, true);
    echo "Created directory: $saveDir\n";
}

// 2. Generate the list of note filenames dynamically
echo "Generating list of note URLs...\n";
$noteNames = ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B'];
$files = [];
foreach ($noteNames as $note) {
    // Octaves 0 through 7 (Gb7 is last)
    for ($oct = 0; $oct <= 7; $oct++) {
        // If we hit Gb7, include and break
        if ($note === 'Gb' && $oct === 7) {
            $files[] = "$note$oct.mp3";
            break;
        }
        $files[] = "$note$oct.mp3";
    }
}

// 4. Download each .mp3
foreach ($files as $fileName) {
    $fileUrl    = $baseUrl . $fileName;
    $localPath  = $saveDir . $fileName;

    // Skip if already downloaded
    if (file_exists($localPath)) {
        echo "Skipping (already exists): $fileName\n";
        continue;
    }

    echo "Downloading $fileName ... ";
    $data = @file_get_contents($fileUrl);
    if ($data === false) {
        echo "FAILED\n";
        continue;
    }

    file_put_contents($localPath, $data);
    echo "OK\n";
}

echo "Download complete. Files saved to $saveDir\n";