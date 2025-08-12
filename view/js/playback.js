// playback.js - Playback control functions
import { showNotification } from './notifications.js';

export function playRandomFile(folder = '/music') {
    console.log('Playing random file from:', folder);
    const playButton = document.querySelector('.play-button');
    const originalText = playButton.textContent;
    playButton.textContent = 'Playing...';
    playButton.disabled = true;
    
    fetch('/random' + (folder !== '/music' ? '?folder=' + folder : ''))
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                showNotification(data.message, 'success');
                setTimeout(window.updateStatus, 1000);
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

export function stopPlayback() {
    console.log('Stopping playback');
    const stopButton = document.querySelector('.stop-button');
    const originalText = stopButton.textContent;
    stopButton.textContent = 'Stopping...';
    stopButton.disabled = true;
    
    fetch('/stop')
        .then(response => response.json())
        .then(data => {
            showNotification(data.message, 'success');
            setTimeout(window.updateStatus, 500);
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

export function pausePlayback() {
    console.log('Pausing playback');
    const pauseButton = document.querySelector('.pause-button');
    const originalText = pauseButton.textContent;
    pauseButton.textContent = 'Pausing...';
    pauseButton.disabled = true;
    
    fetch('/pause')
        .then(response => response.json())
        .then(data => {
            showNotification(data.message, data.status === 'success' ? 'success' : 'info');
            setTimeout(window.updateStatus, 500);
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

export function resumePlayback() {
    console.log('Resuming playback');
    const resumeButton = document.querySelector('.resume-button');
    const originalText = resumeButton.textContent;
    resumeButton.textContent = 'Resuming...';
    resumeButton.disabled = true;
    
    fetch('/resume')
        .then(response => response.json())
        .then(data => {
            showNotification(data.message, 'success');
            setTimeout(window.updateStatus, 500);
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
