import hashlib

def generate_md5_checksum(file_path):
    """
    Calculates the MD5 hash of a file by reading it in chunks.
    """
    # 1. Initialize the MD5 hash object
    md5_hash = hashlib.md5()

    try:
        # 2. Open the file in binary mode ('rb')
        with open(file_path, "rb") as f:
            # 3. Read the file in 4KB chunks
            # Using a loop prevents memory crashes on large files
            for byte_block in iter(lambda: f.read(4096), b""):
                md5_hash.update(byte_block)
        
        # 4. Return the final hex string
        return md5_hash.hexdigest()

    except FileNotFoundError:
        return "Error: File not found."
    except Exception as e:
        return f"An error occurred: {e}"

# Example usage:
# print(generate_md5_checksum("my_installer.exe"))