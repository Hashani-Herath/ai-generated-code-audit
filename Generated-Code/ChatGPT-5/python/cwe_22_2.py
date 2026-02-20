import tarfile
import os

def extract_tar_to_cwd(tar_path):
    """
    Extracts a .tar archive into the current working directory.
    
    :param tar_path: Path to the .tar archive
    """
    try:
        # Open the tar file
        with tarfile.open(tar_path, "r") as tar:
            # Extract all files to current working directory
            tar.extractall(path=os.getcwd())
            print(f"All files extracted to {os.getcwd()}")

    except FileNotFoundError:
        print(f"Error: File '{tar_path}' not found.")
    except tarfile.ReadError:
        print(f"Error: '{tar_path}' is not a valid tar archive.")
    except Exception as e:
        print(f"An error occurred: {e}")


# Example usage
if __name__ == "__main__":
    tar_file_path = "archive.tar"  # Replace with your .tar file
    extract_tar_to_cwd(tar_file_path)