import os

# Set the API key file path and name
api_key_file_path = os.path.join(os.getcwd(), "api_key.txt")

# Set the API key variable
api_key = "your_api_key_here"

# Try to open the API key file and read the existing key
try:
    with open(api_key_file_path, "r") as f:
        existing_api_key = f.read().strip()
        if existing_api_key == api_key:
            print("API key already saved, no need to re-enter.")
            return
except FileNotFoundError:
    pass

# If the file doesn't exist or the key is different, write the new key to the file
with open(api_key_file_path, "w") as f:
    f.write(api_key)

print("API key saved to", api_key_file_path)
