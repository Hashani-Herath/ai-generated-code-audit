import tarfile
import os

def extract_tar_securely(tar_path, dest_dir="."):
    """
    Extracts a .tar archive securely to prevent directory traversal attacks.
    Defaults to extracting in the current working directory (".").
    """
    # 1. Ensure the destination directory exists and get its absolute path
    os.makedirs(dest_dir, exist_ok=True)
    target_dir = os.path.abspath(dest_dir)

    try:
        # 2. Open the tar archive (the 'r:*' mode handles standard, gzip, and bzip2 compression)
        with tarfile.open(tar_path, 'r:*') as tar_ref:
            
            # 3. Iterate through every item inside the archive
            for member in tar_ref.getmembers():
                
                # Calculate what the final absolute path WOULD be on your system
                extracted_path = os.path.abspath(os.path.join(target_dir, member.name))
                
                # 4. Security Check: Ensure the file stays strictly inside the target directory
                if not extracted_path.startswith(target_dir + os.sep):
                    print(f"[!] Security Warning: Blocked malicious file path -> {member.name}")
                    continue
                
                # 5. Extract the file only if it passed the check
                tar_ref.extract(member, target_dir)
                
        print(f"[*] Successfully extracted safe contents to: {target_dir}")
        return True

    except tarfile.TarError as e:
        print(f"[!] Error: The file is not a valid tar archive or is corrupted. Details: {e}")
        return False
    except FileNotFoundError:
        print(f"[!] Error: The archive '{tar_path}' was not found.")
        return False
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
        return False

# --- Example Usage ---
# extract_tar_securely("user_upload.tar.gz")