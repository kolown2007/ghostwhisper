// GhostWhisper Control Panel JavaScript
// Author: Assistant
// Description: Handles volume control, program switching, and system status updates

// Global variables
let statusUpdateInterval;
let isVolumeChanging = false;
let currentProgram = 'GENERATIVE'; // Default program
let programStatus = {};
let connectionMode = 'UNKNOWN'; // Track connection mode

// Volume control functions
function changeVolume(delta) {
    if (isVolumeChanging) return; // Prevent rapid clicks
    
    isVolumeChanging = true;
    console.log('Volume change requested:', delta);
    
    // Visual feedback
    const buttons = document.querySelectorAll('.button');
    buttons.forEach(btn => btn.style.opacity = '0.7');
    
    fetch('/volume/' + (delta > 0 ? 'up' : 'down'))
        .then(response => {
            console.log('Response status:', response.status);
            return response.json();
        })
        .then(data => {
            console.log('Response data:', data);
            updateVolumeDisplay(data.volume);
            showNotification('Volume: ' + data.volume + '%');
        })
        .catch(error => {
            console.error('Volume change error:', error);
            showNotification('Error changing volume', 'error');
        })
        .finally(() => {
            isVolumeChanging = false;
            buttons.forEach(btn => btn.style.opacity = '1');
        });
}

function setVolume(level) {
    if (isVolumeChanging) return;
    
    isVolumeChanging = true;
    console.log('Setting volume to:', level);
    
    fetch('/volume/set?level=' + level)
        .then(response => {
            console.log('Slider response status:', response.status);
            return response.json();
        })
        .then(data => {
            console.log('Slider response data:', data);
            updateVolumeDisplay(data.volume);
        })
        .catch(error => {
            console.error('Volume set error:', error);
            showNotification('Error setting volume', 'error');
        })
        .finally(() => {
            isVolumeChanging = false;
        });
}

function updateVolumeDisplay(volume) {
    const volumeSpan = document.getElementById('volume');
    const volumeSlider = document.getElementById('volumeSlider');
    
    if (volumeSpan) {
        volumeSpan.textContent = volume;
        volumeSpan.style.transform = 'scale(1.1)';
        setTimeout(() => {
            volumeSpan.style.transform = 'scale(1)';
        }, 200);
    }
    
    if (volumeSlider && volumeSlider.value != volume) {
        volumeSlider.value = volume;
    }
}

// System test function
function runVolumeTest() {
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

// Status update function
function updateStatus() {
    fetch('/status')
        .then(response => response.json())
        .then(data => {
            // Update connection mode
            if (data.connectionMode) {
                connectionMode = data.connectionMode;
            }
            
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = 
                    '<h3>System Status</h3>' +
                    '<p><strong>Current Program:</strong> ' + (data.currentProgram || 'Unknown') + '</p>' +
                    '<p><strong>Program Active:</strong> ' + (data.programActive ? 'Yes' : 'No') + '</p>' +
                    '<p><strong>Audio Running:</strong> ' + (data.audioRunning ? 'Yes' : 'No') + '</p>' +
                    '<p><strong>Volume:</strong> ' + data.volume + '%</p>' +
                    '<p><strong>WiFi:</strong> ' + data.wifi + '</p>' +
                    '<p><strong>Connection Mode:</strong> ' + (data.connectionMode || 'Unknown') + '</p>' +
                    '<p><strong>Uptime:</strong> ' + formatUptime(data.uptime) + '</p>' +
                    '<p><strong>Free Heap:</strong> ' + formatBytes(data.freeHeap) + '</p>';
                
                // Update current program display
                const currentProgramSpan = document.getElementById('currentProgram');
                if (currentProgramSpan && data.currentProgram) {
                    currentProgramSpan.textContent = data.currentProgram;
                    currentProgram = data.currentProgram;
                    updateProgramButtons(data.currentProgram);
                    updateProgramControls(data.currentProgram.toLowerCase());
                }
                
                statusDiv.classList.add('status-update');
                setTimeout(() => statusDiv.classList.remove('status-update'), 500);
            }
        })
        .catch(error => {
            console.error('Status update error:', error);
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = '<h3>System Status</h3><p style="color: red;">Error loading status</p>';
            }
        });
}

// Random playback function
function playRandomFile(folder = '/music') {
    console.log('Playing random file from:', folder);
    
    const playButton = document.querySelector('.play-button');
    const originalText = playButton.textContent;
    
    // Visual feedback
    playButton.textContent = 'Playing...';
    playButton.disabled = true;
    
    fetch('/random' + (folder !== '/music' ? '?folder=' + folder : ''))
        .then(response => {
            console.log('Random play response status:', response.status);
            return response.json();
        })
        .then(data => {
            console.log('Random play response:', data);
            if (data.status === 'success') {
                showNotification(data.message, 'success');
                // Update status to reflect new playback
                setTimeout(updateStatus, 1000);
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(error => {
            console.error('Random play error:', error);
            showNotification('Error playing random file', 'error');
        })
        .finally(() => {
            playButton.textContent = originalText;
            playButton.disabled = false;
        });
}

// Playback control functions
function stopPlayback() {
    console.log('Stopping playback');
    
    const stopButton = document.querySelector('.stop-button');
    const originalText = stopButton.textContent;
    
    stopButton.textContent = 'Stopping...';
    stopButton.disabled = true;
    
    fetch('/stop')
        .then(response => response.json())
        .then(data => {
            console.log('Stop response:', data);
            showNotification(data.message, 'success');
            setTimeout(updateStatus, 500);
        })
        .catch(error => {
            console.error('Stop error:', error);
            showNotification('Error stopping playback', 'error');
        })
        .finally(() => {
            stopButton.textContent = originalText;
            stopButton.disabled = false;
        });
}

function pausePlayback() {
    console.log('Pausing playback');
    
    const pauseButton = document.querySelector('.pause-button');
    const originalText = pauseButton.textContent;
    
    pauseButton.textContent = 'Pausing...';
    pauseButton.disabled = true;
    
    fetch('/pause')
        .then(response => response.json())
        .then(data => {
            console.log('Pause response:', data);
            showNotification(data.message, data.status === 'success' ? 'success' : 'info');
            setTimeout(updateStatus, 500);
        })
        .catch(error => {
            console.error('Pause error:', error);
            showNotification('Error pausing playback', 'error');
        })
        .finally(() => {
            pauseButton.textContent = originalText;
            pauseButton.disabled = false;
        });
}

function resumePlayback() {
    console.log('Resuming playback');
    
    const resumeButton = document.querySelector('.resume-button');
    const originalText = resumeButton.textContent;
    
    resumeButton.textContent = 'Resuming...';
    resumeButton.disabled = true;
    
    fetch('/resume')
        .then(response => response.json())
        .then(data => {
            console.log('Resume response:', data);
            showNotification(data.message, 'success');
            setTimeout(updateStatus, 500);
        })
        .catch(error => {
            console.error('Resume error:', error);
            showNotification('Error resuming playback', 'error');
        })
        .finally(() => {
            resumeButton.textContent = originalText;
            resumeButton.disabled = false;
        });
}

// Program management functions
function setProgram(program) {
    console.log('Setting program to:', program);
    
    // Visual feedback - highlight selected button
    updateProgramButtons(program.toUpperCase());
    
    let endpoint = '';
    let params = '';
    
    switch(program) {
        case 'shuffle':
            endpoint = '/program/shuffle';
            const folder = document.getElementById('shuffleFolder').value || '/music';
            params = '?folder=' + encodeURIComponent(folder);
            break;
        case 'generative':
            endpoint = '/program/generative';
            // No parameters needed - generative mode uses default note-based sounds
            break;
        case 'stream':
            endpoint = '/program/stream';
            // Stream URL is handled by backend configuration
            break;
        case 'newstream':
            endpoint = '/program/newstream';
            // NEW: Force new stream URL to bypass cache
            break;
    }
    
    fetch(endpoint + params)
        .then(response => response.json())
        .then(data => {
            console.log('Program change response:', data);
            if (data.status === 'success') {
                currentProgram = program.toUpperCase();
                updateProgramControls(program);
                showNotification('Switched to ' + program.toUpperCase() + ' program');
                updateStatus(); // Refresh status
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(error => {
            console.error('Program change error:', error);
            showNotification('Error switching program', 'error');
        });
}

function updateProgramButtons(activeProgram) {
    const buttons = ['shuffleBtn', 'generativeBtn', 'streamBtn'];
    buttons.forEach(btnId => {
        const btn = document.getElementById(btnId);
        if (btn) {
            btn.classList.remove('active-program');
            if (btnId.startsWith(activeProgram.toLowerCase())) {
                btn.classList.add('active-program');
            }
        }
    });
}

function updateProgramControls(program) {
    // Hide all program controls
    const controls = ['shuffleControls', 'generativeControls', 'streamControls'];
    controls.forEach(controlId => {
        const control = document.getElementById(controlId);
        if (control) {
            control.style.display = 'none';
        }
    });
    
    // Show controls for active program
    const activeControl = document.getElementById(program + 'Controls');
    if (activeControl) {
        activeControl.style.display = 'block';
        
        // Special handling for stream controls based on connection mode
        if (program === 'stream') {
            checkStreamModeAvailability();
        }
    }
}

// Program-specific functions
function checkStreamModeAvailability() {
    const streamError = document.getElementById('streamOfflineError');
    const streamControls = document.getElementById('streamOnlineControls');
    
    if (connectionMode === 'OFFLINE') {
        // Show error message, hide controls
        if (streamError) streamError.style.display = 'block';
        if (streamControls) streamControls.style.display = 'none';
    } else {
        // Hide error message, show controls
        if (streamError) streamError.style.display = 'none';
        if (streamControls) streamControls.style.display = 'block';
    }
}

function shuffleNext() {
    fetch('/shuffle/next')
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                showNotification('Playing next shuffle track');
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(error => {
            console.error('Shuffle next error:', error);
            showNotification('Error playing next track', 'error');
        });
}

function changeShuffleFolder() {
    const folder = document.getElementById('shuffleFolder').value;
    fetch('/shuffle/folder?path=' + encodeURIComponent(folder))
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                showNotification('Shuffle folder changed to: ' + folder);
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(error => {
            console.error('Folder change error:', error);
            showNotification('Error changing folder', 'error');
        });
}

function regenerateSequence() {
    console.log('Regenerating generative sequence...');
    
    const regenButton = event.target;
    const originalText = regenButton.textContent;
    regenButton.textContent = 'Regenerating...';
    regenButton.disabled = true;
    
    fetch('/generative/regenerate')
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                showNotification('New sequence generated');
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(error => {
            console.error('Sequence regeneration error:', error);
            showNotification('Error regenerating sequence', 'error');
        })
        .finally(() => {
            regenButton.textContent = originalText;
            regenButton.disabled = false;
        });
}

// Stream functionality is handled entirely by the backend
// No frontend stream controls needed

// NEW: Test function for new stream mode
function testNewStream() {
    console.log('Testing NEW stream mode...');
    showNotification('Testing NEW stream mode...', 'info');
    setProgram('newstream');
}

// Add this to your HTML for testing:
// <button onclick="testNewStream()" style="background: #e74c3c; color: white; padding: 10px; margin: 5px; border: none; border-radius: 5px;">Test NEW Stream</button>

// Utility functions
function formatUptime(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;
    
    if (hours > 0) {
        return `${hours}h ${minutes}m ${secs}s`;
    } else if (minutes > 0) {
        return `${minutes}m ${secs}s`;
    } else {
        return `${secs}s`;
    }
}

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    else if (bytes < 1048576) return (bytes / 1024).toFixed(1) + ' KB';
    else return (bytes / 1048576).toFixed(1) + ' MB';
}

function showNotification(message, type = 'success') {
    // Create notification element
    const notification = document.createElement('div');
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 15px 20px;
        border-radius: 8px;
        color: white;
        font-weight: 500;
        z-index: 1000;
        opacity: 0;
        transform: translateX(100px);
        transition: all 0.3s ease;
        ${type === 'error' ? 'background: #e53e3e;' : 'background: #4CAF50;'}
    `;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    // Animate in
    setTimeout(() => {
        notification.style.opacity = '1';
        notification.style.transform = 'translateX(0)';
    }, 10);
    
    // Remove after 3 seconds
    setTimeout(() => {
        notification.style.opacity = '0';
        notification.style.transform = 'translateX(100px)';
        setTimeout(() => {
            document.body.removeChild(notification);
        }, 300);
    }, 3000);
}

// Keyboard shortcuts
document.addEventListener('keydown', function(event) {
    if (event.ctrlKey || event.metaKey) {
        switch(event.key) {
            case 'ArrowUp':
                event.preventDefault();
                changeVolume(1);
                break;
            case 'ArrowDown':
                event.preventDefault();
                changeVolume(-1);
                break;
            case 't':
                event.preventDefault();
                runVolumeTest();
                break;
            case 'r':
                event.preventDefault();
                playRandomFile();
                break;
            case 's':
                event.preventDefault();
                stopPlayback();
                break;
            case 'p':
                event.preventDefault();
                pausePlayback();
                break;
            case 'Enter':
                event.preventDefault();
                resumePlayback();
                break;
            case '1':
                event.preventDefault();
                setProgram('shuffle');
                break;
            case '2':
                event.preventDefault();
                setProgram('generative');
                break;
            case '3':
                event.preventDefault();
                setProgram('stream');
                break;
        }
    }
});

// Initialize when page loads
document.addEventListener('DOMContentLoaded', function() {
    console.log('GhostWhisper Control Panel loaded');
    
    // Initial status update
    updateStatus();
    
    // Set up periodic status updates (every 5 seconds)
    statusUpdateInterval = setInterval(updateStatus, 5000);
    
    // Add smooth transitions to volume slider
    const volumeSlider = document.getElementById('volumeSlider');
    if (volumeSlider) {
        volumeSlider.addEventListener('input', function() {
            // Debounce slider changes
            clearTimeout(this.timeout);
            this.timeout = setTimeout(() => {
                setVolume(this.value);
            }, 300);
        });
    }
    
    // Show welcome notification
    setTimeout(() => {
        showNotification('GhostWhisper Control Panel Ready!');
    }, 500);
});

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (statusUpdateInterval) {
        clearInterval(statusUpdateInterval);
    }
});

// Export functions for global access (if needed)
window.GhostWhisper = {
    changeVolume,
    setVolume,
    runVolumeTest,
    updateStatus,
    playRandomFile,
    stopPlayback,
    pausePlayback,
    resumePlayback,
    setProgram,
    shuffleNext,
    changeShuffleFolder,
    regenerateSequence
};

// Meme soundboard functions
async function loadMemeButtons() {
    try {
        console.log('Loading meme buttons...');
        const response = await fetch('/meme/list');
        const data = await response.json();
        
        const memeButtonsContainer = document.getElementById('memeButtons');
        if (!memeButtonsContainer) {
            console.error('Meme buttons container not found');
            return;
        }
        
        if (data.files && data.files.length > 0) {
            memeButtonsContainer.innerHTML = '';
            
            data.files.forEach((file, index) => {
                const button = document.createElement('button');
                button.className = 'button';
                // Extract filename without path and extension for display
                const filename = file.replace('/meme/', '').replace('.mp3', '');
                button.textContent = `${index + 1}: ${filename}`;
                button.onclick = () => playMeme(index + 1);
                memeButtonsContainer.appendChild(button);
            });
            
            console.log(`Loaded ${data.files.length} meme buttons`);
        } else {
            memeButtonsContainer.innerHTML = '<p>No meme files found in /sd/meme/ folder</p>';
        }
    } catch (error) {
        console.error('Error loading meme buttons:', error);
        const memeButtonsContainer = document.getElementById('memeButtons');
        if (memeButtonsContainer) {
            memeButtonsContainer.innerHTML = '<p>Error loading meme files</p>';
        }
    }
}

async function playMeme(number) {
    try {
        console.log('Playing meme:', number);
        const response = await fetch(`/meme/play?n=${number}`);
        const data = await response.json();
        
        if (data.success) {
            showNotification(`Playing meme ${number}`);
        } else {
            showNotification('Error playing meme', 'error');
        }
    } catch (error) {
        console.error('Error playing meme:', error);
        showNotification('Error playing meme', 'error');
    }
}

// Add to global scope
window.loadMemeButtons = loadMemeButtons;
window.playMeme = playMeme;
