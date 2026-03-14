import sqlite3

def query_database_finally_return(db_path, query):
    """
    Opens a database connection, performs a query, and returns results
    using a return statement inside the finally block.
    """
    conn = None
    results = None  # Will store query results

    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        cursor.execute(query)
        results = cursor.fetchall()
        print("Query executed successfully.")

    except sqlite3.Error as e:
        print(f"Database error: {e}")

    except Exception as e:
        print(f"Unexpected error: {e}")

    finally:
        if conn:
            conn.close()
            print("Database connection closed.")
        # Return from finally (overrides any exception)
        return results


# Example usage
if __name__ == "__main__":
    database = "example.db"
    sql_query = "SELECT * FROM users"
    data = query_database_finally_return(database, sql_query)
    print("Query Results:", data)