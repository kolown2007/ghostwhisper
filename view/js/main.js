// main.js - Main initialization and global variables
import { updateStatus } from './status.js';
import { showNotification } from './notifications.js';

// Global variables
export let statusUpdateInterval;
export let isVolumeChanging = false;
export let currentProgram = 'GENERATIVE';
export let programStatus = {};
export let connectionMode = 'UNKNOWN';

export function initializeApp() {
    console.log('GhostWhisper Control Panel loaded');
    
    // Initial status update
    updateStatus();
    
    // Set up periodic status updates (every 5 seconds)
    statusUpdateInterval = setInterval(updateStatus, 5000);
    
    // Add smooth transitions to volume slider
    const volumeSlider = document.getElementById('volumeSlider');
    if (volumeSlider) {
        volumeSlider.addEventListener('input', function() {
            clearTimeout(this.timeout);
            this.timeout = setTimeout(() => {
                window.setVolume(this.value);
            }, 300);
        });
    }
    
    // Show welcome notification
    setTimeout(() => {
        showNotification('GhostWhisper Control Panel Ready!');
    }, 500);
}

export function cleanupApp() {
    if (statusUpdateInterval) {
        clearInterval(statusUpdateInterval);
    }
}

export function runVolumeTest() {
    console.log('Running volume test...');
    const testButton = event.target;
    const originalText = testButton.textContent;
    testButton.textContent = 'Testing...';
    testButton.disabled = true;
    
    fetch('/test')
        .then(response => response.text())
        .then(data => {
            console.log('Volume test result:', data);
            showNotification('Volume test completed. Check console for results.');
        })
        .catch(error => {
            console.error('Volume test error:', error);
            showNotification('Error running volume test', 'error');
        })
        .finally(() => {
            testButton.textContent = originalText;
            testButton.disabled = false;
        });
}
