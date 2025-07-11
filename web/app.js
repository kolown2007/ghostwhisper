// GhostWhisper Control Panel JavaScript
// Author: Assistant
// Description: Handles volume control and system status updates

// Global variables
let statusUpdateInterval;
let isVolumeChanging = false;

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
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = 
                    '<h3>System Status</h3>' +
                    '<p><strong>Volume:</strong> ' + data.volume + '%</p>' +
                    '<p><strong>WiFi:</strong> ' + data.wifi + '</p>' +
                    '<p><strong>Uptime:</strong> ' + formatUptime(data.uptime) + '</p>' +
                    '<p><strong>Free Heap:</strong> ' + formatBytes(data.freeHeap) + '</p>' +
                    '<p><strong>Track:</strong> ' + (data.track ? data.track : 'None') + '</p>';
                
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
    playButton.textContent = '🎵 Playing...';
    playButton.disabled = true;
    
    fetch('/random' + (folder !== '/music' ? '?folder=' + folder : ''))
        .then(response => {
            console.log('Random play response status:', response.status);
            return response.json();
        })
        .then(data => {
            console.log('Random play response:', data);
            if (data.status === 'success') {
                showNotification('🎵 ' + data.message, 'success');
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
                changeVolume(10);
                break;
            case 'ArrowDown':
                event.preventDefault();
                changeVolume(-10);
                break;
            case 't':
                event.preventDefault();
                runVolumeTest();
                break;
            case 'r':
                event.preventDefault();
                playRandomFile();
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
    playRandomFile
};
