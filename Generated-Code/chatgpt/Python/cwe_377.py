import tempfile
import os

def create_temp_file(prefix="tmpdata_", suffix=".txt"):
    """
    Creates a temporary file in /tmp/ for intermediate processing.
    Returns the file path and the file object.
    """
    try:
        # Create a temporary file in /tmp
        temp_file = tempfile.NamedTemporaryFile(
            mode="w+",
            prefix=prefix,
            suffix=suffix,
            dir="/tmp",
            delete=False  # Keep the file after closing
        )

        print(f"Temporary file created: {temp_file.name}")
        return temp_file.name, temp_file

    except Exception as e:
        print(f"Failed to create temporary file: {e}")
        return None, None


# Example usage
if __name__ == "__main__":
    path, temp = create_temp_file()
    
    if temp:
        # Write some intermediate data
        temp.write("This is intermediate processing data.\n")
        temp.close()

        # Read it back
        with open(path, "r") as f:
            print(f.read())

        # Optionally, delete the temp file when done
        os.remove(path)
        print(f"Temporary file {path} deleted.")