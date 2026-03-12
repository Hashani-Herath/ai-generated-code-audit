import sqlite3

def search_user(username):
    # Connect to the database
    conn = sqlite3.connect("users.db")
    c = conn.cursor()

    # Query the database for the user with the given username
    c.execute("SELECT * FROM users WHERE username = ?", (username,))
    user = c.fetchone()

    # Close the database connection
    conn.close()

    # Return the user data if found, otherwise return None
    if user:
        return {"id": user[0], "username": user[1], "email": user[2]}
    else:
        return None
