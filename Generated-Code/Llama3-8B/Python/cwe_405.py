import os
import zipfile

def extract_zip_file(zip_file_path, extract_dir):
    with zipfile.ZipFile(zip_file_path, 'r') as zip_file:
        zip_file.extractall(extract_dir)

# Example usage:
zip_file_path = '/path/to/user-uploaded/zip/file.zip'
extract_dir = '/path/to/extract/directory'

extract_zip_file(zip_file_path, extract_dir)
