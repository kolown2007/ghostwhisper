// program.js - Program management functions
import { showNotification } from './notifications.js';

export function setProgram(program) {
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
            break;
        case 'stream':
            endpoint = '/program/stream';
            break;
        case 'newstream':
            endpoint = '/program/newstream';
            break;
    }
    fetch(endpoint + params)
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                window.currentProgram = program.toUpperCase();
                updateProgramControls(program);
                showNotification('Switched to ' + program.toUpperCase() + ' program');
                window.updateStatus();
            } else {
                showNotification('Error: ' + data.message, 'error');
            }
        })
        .catch(() => showNotification('Error switching program', 'error'));
}

export function updateProgramButtons(activeProgram) {
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

export function updateProgramControls(program) {
    const controls = ['shuffleControls', 'generativeControls', 'streamControls'];
    controls.forEach(controlId => {
        const control = document.getElementById(controlId);
        if (control) control.style.display = 'none';
    });
    const activeControl = document.getElementById(program + 'Controls');
    if (activeControl) {
        activeControl.style.display = 'block';
        if (program === 'stream') {
            checkStreamModeAvailability();
        }
    }
}

function checkStreamModeAvailability() {
    const streamError = document.getElementById('streamOfflineError');
    const streamControls = document.getElementById('streamOnlineControls');
    if (window.connectionMode === 'OFFLINE') {
        if (streamError) streamError.style.display = 'block';
        if (streamControls) streamControls.style.display = 'none';
    } else {
        if (streamError) streamError.style.display = 'none';
        if (streamControls) streamControls.style.display = 'block';
    }
}
