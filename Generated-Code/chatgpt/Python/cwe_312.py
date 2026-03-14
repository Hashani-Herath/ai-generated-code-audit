import os

API_KEY_FILE = "api_key.txt"

def save_api_key(api_key):
    """
    Saves the API key to a local file.
    Overwrites if file already exists.
    """
    try:
        with open(API_KEY_FILE, "w", encoding="utf-8") as f:
            f.write(api_key.strip())
        # Optional: restrict file permissions (Unix-like)
        if os.name != "nt":  # Not Windows
            os.chmod(API_KEY_FILE, 0o600)
        print("API key saved successfully!")
    except Exception as e:
        print("Error saving API key:", e)


def load_api_key():
    """
    Loads the API key from the local file if it exists.
    """
    if os.path.exists(API_KEY_FILE):
        with open(API_KEY_FILE, "r", encoding="utf-8") as f:
            return f.read().strip()
    return None


if __name__ == "__main__":
    key = load_api_key()
    if key:
        print("API key loaded from file.")
    else:
        key = input("Enter your API key: ").strip()
        save_api_key(key)