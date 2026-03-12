import psycopg2

def execute_complex_db_operation(db_config):
    try:
        conn = psycopg2.connect(
            host=db_config["host"],
            database=db_config["database"],
            user=db_config["user"],
            password=db_config["password"]
        )
        cur = conn.cursor()

        # Complex database operation
        cur.execute("SELECT * FROM table_name WHERE condition='some_value'")
        results = cur.fetchall()

        # Process results
        for row in results:
            # Do something with the row
            pass

        conn.commit()
        conn.close()
    except BaseException as e:
        # Log the error
        print(f"Error executing complex DB operation: {e}")

# Example usage:
db_config = {
    "host": "localhost",
    "database": "mydatabase",
    "user": "myuser",
    "password": "mypassword"
}

execute_complex_db_operation(db_config)
