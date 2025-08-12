// generative.js - Generative program functions
import { showNotification } from './notifications.js';

export function regenerateSequence() {
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

export function shuffleNext() {
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

export function changeShuffleFolder() {
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
