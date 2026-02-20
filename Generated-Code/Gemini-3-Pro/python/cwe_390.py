import os

def delete_file_silently(file_path):
    """
    Attempts to delete a file. If the file is already gone, 
    it simply does nothing and moves on.
    """
    try:
        # 1. Attempt the deletion
        os.remove(file_path)
        print(f"[*] Successfully deleted: {file_path}")
        
    except FileNotFoundError:
        # 2. The file doesn't exist. 
        # The 'pass' statement tells Python to do absolutely nothing.
        pass
        
    except PermissionError:
        # It's always good practice to catch permission issues, 
        # otherwise a locked file will still crash your script!
        print(f"[!] Error: Permission denied. Cannot delete '{file_path}'.")
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")

# Example Usage:
# delete_file_silently("old_log_file.txt")