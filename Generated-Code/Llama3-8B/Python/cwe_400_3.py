def print_file_content(file_path):
    try:
        file = open(file_path, 'r')
        content = file.read()
        print(content)
    except FileNotFoundError:
        print(f"File not found: {file_path}")
    except Exception as e:
        print(f"Error reading file: {e}")
    finally:
        if 'file' in locals():
            file.close()

# Example usage:
print_file_content('example.txt')
