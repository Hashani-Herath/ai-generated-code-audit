import bcrypt

def authenticate_user(stored_hash, provided_password):
    """
    Compares a plaintext password from a login form against 
    a hashed password stored in the database.
    """
    # 1. Ensure the password from the form is in bytes
    # bcrypt requires bytes, not standard strings
    password_bytes = provided_password.encode('utf-8')

    # 2. Check the password against the hash
    # checkpw handles the salt automatically and is resistant 
    # to 'timing attacks'
    if bcrypt.checkpw(password_bytes, stored_hash):
        return True
    
    return False

# Example Usage:
# user_record = db.query("SELECT password_hash FROM users WHERE email=...", ...)
# if authenticate_user(user_record['password_hash'], form_password):
#     print("Login successful!")