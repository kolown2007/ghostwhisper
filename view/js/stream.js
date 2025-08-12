// stream.js - Simple stream mode functionality
import { showNotification } from './notifications.js';
import { updateStatus } from './status.js';

// Stream state
let streamState = {
    currentStation: null,
    availableStations: [],
    currentIndex: 0
};

/**
 * Initialize stream mode - fetch stations from API
 */
export function initializeStreamMode() {
    console.log('Initializing stream mode...');
    fetchStationsFromAPI();
    updateStreamUI();
}

/**
 * Fetch stations from remote API (frontend)
 */
async function fetchStationsFromAPI() {
    try {
        console.log('Fetching stations from API...');
        showNotification('Loading stations...', 'info');
        
        const response = await fetch('https://kolown.net/api/ghostwhisper_playlist');
        
        if (!response.ok) {
            throw new Error('Failed to fetch stations');
        }
        
        const data = await response.json();
        
        // Convert API response to station array
        // API format: {"dayang": "https://kolown.net/storage/projects/whisper/Dayang%20Dayang.mp3"}
        streamState.availableStations = Object.entries(data).map(([name, url]) => ({
            name: name,
            url: url
        }));
        
        console.log(`Loaded ${streamState.availableStations.length} stations from API`);
        showNotification(`Loaded ${streamState.availableStations.length} stations`, 'success');
        
        // Set first station as current
        if (streamState.availableStations.length > 0) {
            streamState.currentStation = streamState.availableStations[0].name;
            streamState.currentIndex = 0;
        }
        
        updateStreamUI();
        
    } catch (error) {
        console.error('Error fetching stations:', error);
        showNotification('Error loading stations from API', 'error');
        
        // Fallback to hardcoded stations
        streamState.availableStations = [
            { name: "Smooth Jazz", url: "http://jazz-wr04.ice.infomaniak.ch/jazz-wr04-128.mp3" },
            { name: "Reggae", url: "http://reggae.stream.laut.fm/reggae" }
        ];
        streamState.currentStation = streamState.availableStations[0].name;
        streamState.currentIndex = 0;
        updateStreamUI();
    }
}

/**
 * Go to next station
 */
export function nextStation() {
    if (streamState.availableStations.length === 0) {
        showNotification('No stations available', 'error');
        return;
    }
    
    // Move to next station (wrap around to beginning)
    streamState.currentIndex = (streamState.currentIndex + 1) % streamState.availableStations.length;
    const nextStation = streamState.availableStations[streamState.currentIndex];
    
    console.log('Switching to next station:', nextStation.name);
    
    // Send station URL to ESP32
    fetch('/stream/play', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            url: nextStation.url,
            name: nextStation.name
        })
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            streamState.currentStation = nextStation.name;
            showNotification(`Now playing: ${nextStation.name}`, 'success');
            updateStreamUI();
            setTimeout(updateStatus, 1000);
        } else {
            showNotification('Error switching station: ' + data.message, 'error');
        }
    })
    .catch(error => {
        console.error('Next station error:', error);
        showNotification('Error switching to next station', 'error');
    });
}

/**
 * Start stream mode
 */
export function startStream() {
    console.log('Starting stream mode...');
    
    if (streamState.availableStations.length === 0) {
        showNotification('No stations available - loading...', 'info');
        fetchStationsFromAPI();
        return;
    }
    
    const currentStation = streamState.availableStations[streamState.currentIndex];
    
    fetch('/program/stream', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            url: currentStation.url,
            name: currentStation.name
        })
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            streamState.currentStation = currentStation.name;
            showNotification(`Stream started: ${currentStation.name}`, 'success');
            updateStreamUI();
        } else {
            showNotification('Error starting stream: ' + data.message, 'error');
        }
        setTimeout(updateStatus, 1000);
    })
    .catch(error => {
        console.error('Stream start error:', error);
        showNotification('Error starting stream', 'error');
    });
}

/**
 * Update stream UI
 */
function updateStreamUI() {
    // Update current station display
    const currentStationElement = document.getElementById('currentStation');
    if (currentStationElement) {
        currentStationElement.textContent = streamState.currentStation || 'No station';
    }
    
    // Update station count
    const stationCountElement = document.getElementById('stationCount');
    if (stationCountElement) {
        stationCountElement.textContent = `${streamState.availableStations.length} stations available`;
    }
    
    // Show/hide next button based on availability
    const nextButton = document.querySelector('.next-station-button');
    if (nextButton) {
        nextButton.style.display = streamState.availableStations.length > 1 ? 'inline-block' : 'none';
    }
}

/**
 * Refresh stations from API
 */
export function refreshStations() {
    showNotification('Refreshing stations...', 'info');
    fetchStationsFromAPI();
}

/**
 * Get current stream info
 */
export function getCurrentStation() {
    return {
        station: streamState.currentStation,
        index: streamState.currentIndex,
        total: streamState.availableStations.length
    };
}
