import sqlite3

def find_user_by_username(username):
    """
    Searches for a user in the SQLite database by username.
    Returns user data if found, otherwise returns None.
    """

    # Connect to database
    conn = sqlite3.connect("app.db")
    cursor = conn.cursor()

    try:
        # Use parameterized query to prevent SQL injection
        cursor.execute("SELECT id, username, email FROM users WHERE username = ?", (username,))
        
        user = cursor.fetchone()

        return user  # Returns tuple (id, username, email) or None

    except sqlite3.Error as e:
        print("Database error:", e)
        return None

    finally:
        conn.close()