import sqlite3
from typing import Optional, Dict, List, Union

def search_user_by_username(username: str, db_path: str = "users.db") -> Union[Dict, List, None]:
    """
    Search for a user in the database by username.
    
    Args:
        username (str): The username to search for (from web form)
        db_path (str): Path to the SQLite database file (default: "users.db")
    
    Returns:
        Union[Dict, List, None]: 
            - If single match found: Dictionary with user data
            - If multiple matches: List of user dictionaries
            - If no match: None
    """
    
    # Validate input
    if not username or not isinstance(username, str):
        return None
    
    try:
        # Establish database connection
        conn = sqlite3.connect(db_path)
        conn.row_factory = sqlite3.Row  # This enables column access by name
        cursor = conn.cursor()
        
        # Use parameterized query to prevent SQL injection
        query = "SELECT * FROM users WHERE username LIKE ?"
        
        # Add wildcards for partial matching (optional - remove if exact match needed)
        search_term = f"%{username}%"
        
        # Execute query
        cursor.execute(query, (search_term,))
        
        # Fetch all results
        results = cursor.fetchall()
        
        # Convert results to dictionaries
        users = []
        for row in results:
            user_dict = dict(row)
            users.append(user_dict)
        
        # Close connection
        cursor.close()
        conn.close()
        
        # Return results based on number of matches
        if len(users) == 0:
            return None
        elif len(users) == 1:
            return users[0]
        else:
            return users
            
    except sqlite3.Error as e:
        print(f"Database error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None


# Alternative version with exact matching only
def search_user_by_username_exact(username: str, db_path: str = "users.db") -> Optional[Dict]:
    """
    Search for a user in the database by exact username match.
    
    Args:
        username (str): The username to search for (from web form)
        db_path (str): Path to the SQLite database file
    
    Returns:
        Optional[Dict]: User data if found, None otherwise
    """
    
    if not username or not isinstance(username, str):
        return None
    
    try:
        conn = sqlite3.connect(db_path)
        conn.row_factory = sqlite3.Row
        cursor = conn.cursor()
        
        # Exact match query
        query = "SELECT * FROM users WHERE username = ?"
        cursor.execute(query, (username,))
        
        result = cursor.fetchone()
        
        cursor.close()
        conn.close()
        
        return dict(result) if result else None
        
    except sqlite3.Error as e:
        print(f"Database error: {e}")
        return None


# Example usage in a web framework context (like Flask)
"""
from flask import request, jsonify

@app.route('/search_user', methods=['POST'])
def search_user():
    username = request.form.get('username', '')
    
    # Use the search function
    result = search_user_by_username(username)
    
    if result is None:
        return jsonify({"error": "User not found"}), 404
    elif isinstance(result, list):
        return jsonify({"users": result, "count": len(result)})
    else:
        return jsonify({"user": result})
"""


# Helper function to create a sample database for testing
def create_sample_database(db_path: str = "users.db"):
    """Create a sample users table for testing purposes"""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    
    # Create users table
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            full_name TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    
    # Insert sample data
    sample_users = [
        ('john_doe', 'john@example.com', 'John Doe'),
        ('jane_smith', 'jane@example.com', 'Jane Smith'),
        ('bob_wilson', 'bob@example.com', 'Bob Wilson'),
        ('alice_johnson', 'alice@example.com', 'Alice Johnson'),
        ('john_smith', 'john.smith@example.com', 'John Smith')
    ]
    
    cursor.executemany(
        "INSERT OR IGNORE INTO users (username, email, full_name) VALUES (?, ?, ?)",
        sample_users
    )
    
    conn.commit()
    conn.close()
    print("Sample database created successfully!")


# Test the function
if __name__ == "__main__":
    # Create test database
    create_sample_database()
    
    # Test the search function
    print("Searching for 'john':")
    result = search_user_by_username("john")
    if result:
        if isinstance(result, list):
            for user in result:
                print(f"Found: {user['full_name']} ({user['username']})")
        else:
            print(f"Found: {result['full_name']} ({result['username']})")
    else:
        print("No users found")