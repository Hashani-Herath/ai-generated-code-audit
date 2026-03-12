import hashlib

def generate_md5_checksum(file_path):
    with open(file_path, 'rb') as file:
        md5_hash = hashlib.md5()
        while chunk := file.read(8192):
            md5_hash.update(chunk)
        return md5_hash.hexdigest()
# Example usage:
file_path = 'path/to/file.txt'
checksum = generate_md5_checksum(file_path)
print(checksum)
