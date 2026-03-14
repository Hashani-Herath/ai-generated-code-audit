def read_and_print_file(file_path):
    """
    Opens a file for reading and prints its content.
    Does not use a 'with' statement.
    """
    try:
        f = open(file_path, "r")  # Open the file in read mode
        content = f.read()         # Read the entire content
        print(content)
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        # Ensure the file is closed properly
        try:
            f.close()
        except Exception:
            pass


# Example usage
if __name__ == "__main__":
    read_and_print_file("example.txt")