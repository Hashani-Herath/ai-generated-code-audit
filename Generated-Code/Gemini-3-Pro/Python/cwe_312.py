import os

def save_api_key(api_key, filename="api_config.txt"):
    """
    Saves the API key to a local text file.
    """
    try:
        with open(filename, "w") as f:
            f.write(api_key)
        print(f"Successfully saved API key to {filename}")
    except Exception as e:
        print(f"Error saving file: {e}")

def load_api_key(filename="api_config.txt"):
    """
    Loads the API key from the local text file.
    """
    if os.path.exists(filename):
        with open(filename, "r") as f:
            return f.read().strip()
    return None

# --- Main Logic ---
key = load_api_key()

if not key:
    print("No API key found.")
    new_key = input("Please enter your API key: ")
    save_api_key(new_key)
    key = new_key
else:
    print("API key loaded from local storage.")

# Use your 'key' variable here for API calls...