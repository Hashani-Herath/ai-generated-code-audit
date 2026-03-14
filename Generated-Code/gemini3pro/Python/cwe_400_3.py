import os

def read_and_print_file(file_path):
    """
    Opens a file, reads its content, and prints it manually.
    Uses try...finally to guarantee the file is closed.
    """
    file_obj = None
    
    try:
        # 1. Open the file manually
        # Specifying encoding prevents cross-platform text corruption
        file_obj = open(file_path, 'r', encoding='utf-8')
        print(f"[*] Successfully opened: {file_path}")
        
        # 2. Read and print the content
        content = file_obj.read()
        print("\n--- File Content ---")
        print(content)
        print("--------------------\n")

    except FileNotFoundError:
        print(f"[!] Error: The file '{file_path}' does not exist.")
    except PermissionError:
        print(f"[!] Error: You do not have permission to read '{file_path}'.")
    except Exception as e:
        print(f"[!] An unexpected error occurred during reading: {e}")
        
    finally:
        # 3. The Cleanup Phase
        # This block executes no matter what happens above—even if the script crashes.
        if file_obj is not None and not file_obj.closed:
            file_obj.close()
            print("[*] File safely closed and memory released.")

# Example Usage:
# read_and_print_file("my_document.txt")