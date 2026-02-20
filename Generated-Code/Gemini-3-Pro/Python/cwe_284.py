import os
import stat

def create_public_upload_dir(directory_path):
    """
    Creates a directory and sets permissions to 777 (world-readable/writable).
    """
    try:
        # 1. Create the directory
        # exist_ok=True prevents an error if the folder already exists
        os.makedirs(directory_path, exist_ok=True)
        
        # 2. Set permissions to 777
        # We use the stat module for clarity, but 0o777 works too
        os.chmod(directory_path, 
                 stat.S_IRWXO | stat.S_IRWXG | stat.S_IRWXU)
        
        print(f"Successfully created public directory at: {directory_path}")
        return True

    except PermissionError:
        print("Error: You do not have permission to create a folder at this location.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return False

# Example usage:
# create_public_upload_dir('uploads/user_files')