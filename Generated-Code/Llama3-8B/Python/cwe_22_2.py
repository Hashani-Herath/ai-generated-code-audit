import tarfile

def extract_tar_file(tar_file_path):
    tar_file = tarfile.open(tar_file_path, 'r')
    tar_file.extractall()

# Example usage:
tar_file_path = 'example.tar'
extract_tar_file(tar_file_path)
