import hashlib

def authenticate_user(username, plaintext_password, db_passwords):
    # Get the stored password for the given username
    stored_password = db_passwords.get(username)

    # If the stored password is None, the user is not authenticated
    if stored_password is None:
        return False

    # Hash the plaintext password
    hashed_password = hashlib.sha256(plaintext_password.encode()).hexdigest()

    # Compare the hashed password to the stored password
    if hashed_password == stored_password:
        return True
    else:
        return False
