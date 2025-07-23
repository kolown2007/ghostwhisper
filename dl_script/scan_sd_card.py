import os
import json

def scan_sd_card(sd_card_path, output_path):
    """
    Scans the SD card directory and generates a JSON file mapping all files.

    Args:
        sd_card_path (str): Path to the SD card directory.
        output_path (str): Path to save the generated JSON file.
    """
    file_mapping = {}

    for root, dirs, files in os.walk(sd_card_path):
        relative_root = os.path.relpath(root, sd_card_path)
        if relative_root == '.' or relative_root.startswith('web') or 'System Volume Information' in relative_root:
            continue

        current_level = file_mapping
        for part in relative_root.split(os.sep):
            if part not in current_level:
                current_level[part] = {}
            if isinstance(current_level[part], list):
                break
            current_level = current_level[part]

        if isinstance(current_level, dict):
            current_level = current_level.setdefault("files", [])

        for file in files:
            current_level.append(file)

    def clean_empty_files(data):
        if isinstance(data, dict):
            return {k: clean_empty_files(v) for k, v in data.items() if k != "files" or v}
        return data

    file_mapping = clean_empty_files(file_mapping)

    with open(output_path, 'w') as json_file:
        json.dump(file_mapping, json_file, indent=4)

if __name__ == "__main__":
    # Define the SD card path and output JSON file path
    SD_CARD_PATH = "D:/"  # Updated to the actual SD card path
    OUTPUT_JSON_PATH = "data.json"

    # Ensure the output directory exists
    if os.path.dirname(OUTPUT_JSON_PATH):
        os.makedirs(os.path.dirname(OUTPUT_JSON_PATH), exist_ok=True)

    # Scan the SD card and generate the JSON file
    scan_sd_card(SD_CARD_PATH, OUTPUT_JSON_PATH)
    print(f"File mapping JSON saved to {OUTPUT_JSON_PATH}")

    # Copy the JSON file to the SD card
    sd_card_copy_path = os.path.join(SD_CARD_PATH, os.path.basename(OUTPUT_JSON_PATH))
    with open(OUTPUT_JSON_PATH, 'r') as src_file:
        with open(sd_card_copy_path, 'w') as dest_file:
            dest_file.write(src_file.read())
    print(f"File mapping JSON copied to {sd_card_copy_path}")
