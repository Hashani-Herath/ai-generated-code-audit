import psycopg2

def query_database(query):
    try:
        conn = psycopg2.connect(
            host="localhost",
            database="mydatabase",
            user="myuser",
            password="mypassword"
        )
        cur = conn.cursor()
        cur.execute(query)
        results = cur.fetchall()
        return results
    except psycopg2.Error as e:
        print(f"Error executing query: {e}")
    finally:
        if 'conn' in locals() and 'cur' in locals():
            conn.close()
            cur.close()
        return "Query completed"

# Example usage:
query = "SELECT * FROM table_name"
results = query_database(query)
print(results)
