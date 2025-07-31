import os
import requests

#run this script directly and input the number based on the options

# Fetch and display available soundfont options
print("Fetching available soundfont options...")
try:
    response = requests.get('https://gleitz.github.io/midi-js-soundfonts/FatBoy/names.json')
    response.raise_for_status()
    soundfont_names = response.json()
    
    print("\nAvailable soundfont options:")
    for i, name in enumerate(soundfont_names, 1):
        print(f"{i:2d}. {name}")
    
    print()
except requests.RequestException as e:
    print(f"Could not fetch soundfont list: {e}")
    print("Please enter the folder name manually.")
    soundfont_names = []

# Ask user for folder name or number
if soundfont_names:
    choice = input("Enter the number or folder name: ")
    try:
        # Try to parse as number
        index = int(choice) - 1
        if 0 <= index < len(soundfont_names):
            folder_name = soundfont_names[index]
            print(f"Selected: {folder_name}")
        else:
            print("Invalid number. Please enter the folder name manually.")
            folder_name = input("Enter the soundfont folder name: ")
    except ValueError:
        # Not a number, use as folder name
        folder_name = choice
else:
    folder_name = input("Enter the soundfont folder name: ")
base_url = f'https://gleitz.github.io/midi-js-soundfonts/FatBoy/{folder_name}-mp3/'

# Create directory name with -mp3 suffix
save_dir = os.path.join(os.path.dirname(__file__), f"{folder_name}-mp3")

# 1. Make sure the save directory exists
if not os.path.isdir(save_dir):
    os.makedirs(save_dir, mode=0o755, exist_ok=True)
    print(f"Created directory: {save_dir}")

# 2. Generate the list of note filenames dynamically
print("Generating list of note URLs...")
note_names = ['C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G', 'Ab', 'A', 'Bb', 'B']
files = []

for note in note_names:
    # Octaves 0 through 7 (Gb7 is last)
    for oct in range(8):  # 0 to 7
        # If we hit Gb7, include and break
        if note == 'Gb' and oct == 7:
            files.append(f"{note}{oct}.mp3")
            break
        files.append(f"{note}{oct}.mp3")

# 4. Download each .mp3
for file_name in files:
    file_url = f"{base_url}{file_name}"
    local_path = os.path.join(save_dir, file_name)

    # Skip if already downloaded
    if os.path.exists(local_path):
        print(f"Skipping (already exists): {file_name}")
        continue

    print(f"Downloading {file_name} ... ", end="")
    try:
        response = requests.get(file_url, timeout=10)
        response.raise_for_status()  # Raise an error for HTTP issues
        
        with open(local_path, 'wb') as f:
            f.write(response.content)
        print("OK")
    except requests.RequestException as e:
        print("FAILED")
        continue

print(f"Download complete. Files saved to {folder_name}")
