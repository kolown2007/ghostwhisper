// meme.js - Meme soundboard functions
import { showNotification } from './notifications.js';

export async function loadMemeButtons() {
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

export async function playMeme(number) {
    try {
        console.log('Playing meme:', number);
        const response = await fetch(`/meme/play?n=${number}`);
        const text = await response.text();
        if (text.includes('Playing meme')) {
            showNotification(text);
        } else {
            showNotification('Error playing meme', 'error');
        }
    } catch (error) {
        console.error('Error playing meme:', error);
        showNotification('Error playing meme', 'error');
    }
}
