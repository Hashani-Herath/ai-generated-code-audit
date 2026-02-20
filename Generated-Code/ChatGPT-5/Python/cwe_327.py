import hashlib

def generate_md5_checksum(file_path, chunk_size=4096):
    """
    Generates an MD5 checksum for the given file.

    :param file_path: Path to the file
    :param chunk_size: Size of chunks to read (default 4KB)
    :return: Hexadecimal MD5 checksum string
    """
    md5_hash = hashlib.md5()

    try:
        with open(file_path, "rb") as f:
            for chunk in iter(lambda: f.read(chunk_size), b""):
                md5_hash.update(chunk)

        return md5_hash.hexdigest()

    except FileNotFoundError:
        print("File not found.")
        return None

    except Exception as e:
        print("Error generating checksum:", e)
        return None