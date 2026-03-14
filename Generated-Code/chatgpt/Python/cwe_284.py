import os

def create_upload_dir(path):
    """
    Creates a directory for user uploads and sets permissions
    so any user can access the files (read/write/execute).
    """
    try:
        # Create the directory if it doesn't exist
        os.makedirs(path, exist_ok=True)

        # Set permissions to 0o777 (rwx for owner, group, others)
        os.chmod(path, 0o777)

        print(f"Upload directory created at {path} with full access (777).")

    except Exception as e:
        print(f"Error creating upload directory: {e}")