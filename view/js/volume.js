// volume.js - Volume control functions
import { showNotification } from './notifications.js';

export function changeVolume(delta) {
    if (window.isVolumeChanging) return;
    window.isVolumeChanging = true;
    const buttons = document.querySelectorAll('.button');
    buttons.forEach(btn => btn.style.opacity = '0.7');
    fetch('/volume/' + (delta > 0 ? 'up' : 'down'))
        .then(response => response.json())
        .then(data => {
            updateVolumeDisplay(data.volume);
            showNotification('Volume: ' + data.volume + '%');
        })
        .catch(() => showNotification('Error changing volume', 'error'))
        .finally(() => {
            window.isVolumeChanging = false;
            buttons.forEach(btn => btn.style.opacity = '1');
        });
}

export function setVolume(level) {
    if (window.isVolumeChanging) return;
    window.isVolumeChanging = true;
    fetch('/volume/set?level=' + level)
        .then(response => response.json())
        .then(data => updateVolumeDisplay(data.volume))
        .catch(() => showNotification('Error setting volume', 'error'))
        .finally(() => { window.isVolumeChanging = false; });
}

export function updateVolumeDisplay(volume) {
    const volumeSpan = document.getElementById('volume');
    const volumeSlider = document.getElementById('volumeSlider');
    if (volumeSpan) {
        volumeSpan.textContent = volume;
        volumeSpan.style.transform = 'scale(1.1)';
        setTimeout(() => { volumeSpan.style.transform = 'scale(1)'; }, 200);
    }
    if (volumeSlider && volumeSlider.value != volume) {
        volumeSlider.value = volume;
    }
}
