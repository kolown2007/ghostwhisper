# GhostWhisper Web Interface

This folder contains the web interface files for the GhostWhisper ambient audio system.

## Files Overview

### `index.html`
- Main control panel interface
- Contains template variables that get replaced by the ESP32:
  - `%VOLUME%` - Current volume level
  - `%WIFI_IP%` - WiFi IP address
  - `%UPTIME%` - System uptime in seconds
  - `%FREE_HEAP%` - Available memory

### `style.css`
- Modern, responsive styling
- Gradient backgrounds and smooth animations
- Mobile-friendly design
- Customizable color scheme

### `app.js`
- Volume control functionality
- Real-time status updates
- Keyboard shortcuts (Ctrl+↑/↓ for volume, Ctrl+T for test)
- Notification system
- Error handling and user feedback

## Installation

### Option 1: SD Card (Recommended)
1. Copy this entire `web` folder to your SD card
2. The ESP32 will automatically serve files from `/web/` on the SD card
3. If SD card files aren't found, the system falls back to built-in HTML

### Option 2: Repository Development
1. Edit files in this folder during development
2. Copy to SD card when ready for deployment
3. Use version control to track changes

## Usage

### Web Interface Features
- **Volume Control**: Buttons, slider, and keyboard shortcuts
- **System Status**: Real-time WiFi, uptime, and memory info
- **Volume Test**: Diagnostic tool to verify audio hardware
- **Responsive Design**: Works on desktop and mobile devices

### Keyboard Shortcuts
- `Ctrl + ↑` - Volume up
- `Ctrl + ↓` - Volume down  
- `Ctrl + T` - Run volume test

### API Endpoints
The web interface communicates with these ESP32 endpoints:
- `GET /` - Main control panel
- `GET /volume/up` - Increase volume by 10%
- `GET /volume/down` - Decrease volume by 10%
- `GET /volume/set?level=X` - Set volume to X%
- `GET /status` - Get system status JSON
- `GET /test` - Run volume diagnostic test

## Customization

### Styling
Edit `style.css` to customize:
- Color scheme (change gradient colors)
- Fonts and typography
- Button styles and animations
- Layout and spacing

### Functionality
Edit `app.js` to add:
- New control features
- Different update intervals
- Custom keyboard shortcuts
- Enhanced notifications

### Layout
Edit `index.html` to modify:
- Page structure
- Add new sections
- Change button layouts
- Modify status display

## Template Variables

The ESP32 processes these template variables in HTML files:
- `%VOLUME%` - Current volume (0-100)
- `%WIFI_IP%` - IP address (e.g., "192.168.1.100")
- `%UPTIME%` - Uptime in seconds
- `%FREE_HEAP%` - Free memory in bytes

## Development Tips

1. **Live Testing**: Make changes to files on SD card and refresh browser
2. **Debugging**: Use browser developer tools console for JavaScript errors
3. **Mobile Testing**: Test on phone/tablet for responsive design
4. **Fallback**: Always test that fallback HTML works when SD card is removed

## Browser Compatibility

- **Chrome/Edge**: Full support
- **Firefox**: Full support
- **Safari**: Full support
- **Mobile browsers**: Responsive design optimized

## File Structure on SD Card
```
/web/
├── index.html      # Main control panel
├── style.css       # Styling and animations
├── app.js         # JavaScript functionality
└── README.md      # This documentation
```

## Security Notes

- Files are served directly from SD card
- No server-side processing of user input in static files
- Directory traversal protection implemented in ESP32 code
- All user input sanitized on ESP32 side


php
php -S localhost:8000