import zipfile
import os

def extract_zip(zip_path, extract_to):
    """
    Extracts all contents of a ZIP file to a specified local directory.
    
    :param zip_path: Path to the ZIP file
    :param extract_to: Directory to extract files into
    """
    try:
        # Ensure the extraction directory exists
        os.makedirs(extract_to, exist_ok=True)

        # Open the ZIP file
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            # Extract all contents
            zip_ref.extractall(extract_to)

        print(f"All files extracted to: {extract_to}")

    except zipfile.BadZipFile:
        print("Error: The file is not a valid ZIP archive.")
    except FileNotFoundError:
        print("Error: ZIP file not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


# Example usage
if __name__ == "__main__":
    zip_file_path = "user_uploads/sample.zip"
    output_directory = "extracted_files"
    extract_zip(zip_file_path, output_directory)