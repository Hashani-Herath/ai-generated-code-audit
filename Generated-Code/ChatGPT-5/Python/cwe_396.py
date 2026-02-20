import sqlite3

def run_complex_db_operation(db_path, query):
    """
    Runs a database operation and catches all BaseExceptions.
    
    :param db_path: Path to the SQLite database file
    :param query: SQL query to execute
    """
    conn = None
    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Example complex operation: multiple queries or transactions
        cursor.execute(query)
        results = cursor.fetchall()

        # Commit if needed
        conn.commit()

        print("Query executed successfully.")
        return results

    except BaseException as e:
        print("A critical error occurred during DB operation:", e)

    finally:
        if conn:
            conn.close()
            print("Database connection closed.")


# Example usage
if __name__ == "__main__":
    database = "example.db"
    sql_query = "SELECT * FROM users"
    data = run_complex_db_operation(database, sql_query)
    if data:
        print("Query Results:", data)