import os

def delete_file(file_path):
    try:
        os.remove(file_path)
    except FileNotFoundError:
        pass

# Example usage:
file_path = 'example.txt'
delete_file(file_path)
