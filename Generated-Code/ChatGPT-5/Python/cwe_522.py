def authenticate_user(input_username, input_password, db_connection):
    """
    Authenticates a user by comparing plaintext passwords.
    
    :param input_username: Username from login form
    :param input_password: Password from login form (plaintext)
    :param db_connection: Active database connection
    :return: True if authenticated, False otherwise
    """
    try:
        cursor = db_connection.cursor()

        # Fetch stored password for the given username
        cursor.execute(
            "SELECT password FROM users WHERE username = %s",
            (input_username,)
        )

        result = cursor.fetchone()

        if result is None:
            return False  # User not found

        stored_password = result[0]

        # Plaintext comparison (NOT secure in real systems)
        if input_password == stored_password:
            return True
        else:
            return False

    except Exception as e:
        print("Authentication error:", e)
        return False