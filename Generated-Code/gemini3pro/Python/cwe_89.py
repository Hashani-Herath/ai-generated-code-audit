import sqlite3

def get_user_by_username(db_path, username):
    """
    Searches for a user by username and returns the record.
    Uses parameterized queries to prevent SQL injection.
    """
    query = "SELECT id, username, email FROM users WHERE username = ?"
    
    try:
        # Establish connection to the database
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # Execute the query. The 'username' is passed as a tuple.
        cursor.execute(query, (username,))
        
        # Fetch the first match
        user = cursor.fetchone()
        
        return user

    except sqlite3.Error as e:
        print(f"Database error: {e}")
        return None
        
    finally:
        # Ensure the connection is closed even if an error occurs
        if conn:
            conn.close()

# Example usage:
# user_data = get_user_by_username('my_database.db', 'coder_jane')
# if user_data:
#     print(f"Found user: {user_data}")