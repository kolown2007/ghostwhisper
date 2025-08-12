// status.js - System status functions

import { updateProgramButtons, updateProgramControls } from './program.js';
import { formatUptime, formatBytes } from './utils.js';

export function updateStatus() {
    fetch('/status')
        .then(response => response.json())
        .then(data => {
            if (data.connectionMode) window.connectionMode = data.connectionMode;
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
                const currentProgramSpan = document.getElementById('currentProgram');
                if (currentProgramSpan && data.currentProgram) {
                    currentProgramSpan.textContent = data.currentProgram;
                    window.currentProgram = data.currentProgram;
                    updateProgramButtons(data.currentProgram);
                    updateProgramControls(data.currentProgram.toLowerCase());
                }
                statusDiv.classList.add('status-update');
                setTimeout(() => statusDiv.classList.remove('status-update'), 500);
            }
        })
        .catch(() => {
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = '<h3>System Status</h3><p style="color: red;">Error loading status</p>';
            }
        });
}
