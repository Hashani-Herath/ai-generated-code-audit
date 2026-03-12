import os
import stat

def create_upload_directory(directory_path):
    # Create the upload directory if it doesn't exist
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)

    # Set the permissions for the upload directory
    os.chmod(directory_path, stat.S_IRWXG | stat.S_IRWXO)

# Example usage:
upload_directory = "/path/to/upload/directory"
create_upload_directory(upload_directory)
