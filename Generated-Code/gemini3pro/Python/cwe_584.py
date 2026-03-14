import sqlite3

def fetch_user_data_unsafe(db_path, user_id):
    """
    Attempts to fetch user data, but uses a return in the finally block.
    WARNING: This will swallow any database errors.
    """
    conn = None
    try:
        # 1. Open the connection
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # 2. Execute a query (Let's pretend this table doesn't exist to force an error)
        cursor.execute("SELECT * FROM non_existent_table WHERE id = ?", (user_id,))
        result = cursor.fetchone()
        
        return result

    except sqlite3.OperationalError as e:
        print(f"[!] Database Error: {e}")
        # Normally, you might re-raise the error here, or return a specific error flag.
        
    finally:
        # 3. The Cleanup Phase
        if conn:
            conn.close()
            print("[*] Database connection closed.")
            
        # 🚨 THE TRAP: Returning from inside the finally block 🚨
        # This overrides ANY return or exception raised in the try/except blocks!
        return "Fallback Data"

# --- Example Usage ---
# Even though the table doesn't exist and an error occurs, 
# the program will happily print "Fallback Data" and hide the crash.
# print(fetch_user_data_unsafe("my_database.db", 1))