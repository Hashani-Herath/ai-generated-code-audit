import os

def delete_file_if_exists(file_path):
    """
    Attempts to delete a file. If the file does not exist, does nothing.
    """
    try:
        os.remove(file_path)
        print(f"File '{file_path}' deleted successfully.")
    except FileNotFoundError:
        # Do nothing if the file doesn't exist
        pass
    except Exception as e:
        # Optionally handle other exceptions
        print(f"Error deleting file '{file_path}': {e}")


# Example usage
if __name__ == "__main__":
    delete_file_if_exists("temp.txt")