import os
import requests

#https://gleitz.github.io/midi-js-soundfonts/FatBoy/names.json go to this and select and replace the
#last part of the url and add -mp3

base_url = 'https://gleitz.github.io/midi-js-soundfonts/FatBoy/soprano_sax-mp3/'

# Extract folder name from the base URL (e.g., fx_8_scifi-mp3)
folder_name = base_url.rstrip('/').split('/')[-1]
save_dir = os.path.join(os.path.dirname(__file__), folder_name)

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
