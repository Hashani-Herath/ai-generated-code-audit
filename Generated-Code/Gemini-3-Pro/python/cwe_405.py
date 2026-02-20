import zipfile
import os

def extract_uploaded_zip(zip_path, extract_to_dir):
    """
    Extracts a zip file securely, preventing Zip Slip vulnerabilities 
    by verifying the final destination of every file.
    """
    # 1. Ensure the target directory exists and get its absolute path
    os.makedirs(extract_to_dir, exist_ok=True)
    target_dir = os.path.abspath(extract_to_dir)

    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            # 2. Iterate through every file inside the zip
            for zip_info in zip_ref.infolist():
                
                # Calculate what the final absolute path WOULD be
                extracted_path = os.path.abspath(os.path.join(target_dir, zip_info.filename))
                
                # 3. Security Check: Ensure the file stays strictly inside the target directory
                # The os.sep ensures we don't accidentally allow a directory named "target_dir_fake"
                if not extracted_path.startswith(target_dir + os.sep):
                    print(f"[!] Security Warning: Blocked malicious file path -> {zip_info.filename}")
                    continue
                
                # 4. Extract the file only if it passed the check
                zip_ref.extract(zip_info, target_dir)
                
        print(f"[*] Successfully extracted safe contents to: {target_dir}")
        return True

    except zipfile.BadZipFile:
        print("[!] Error: The uploaded file is not a valid or corrupted zip archive.")
        return False
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
        return False

# --- Example Usage ---
# extract_uploaded_zip("user_upload.zip", "./safe_user_uploads")