import psycopg2
from psycopg2 import Extras
import os

def connect_to_postgres():
    """
    Establishes a connection to a PostgreSQL database using 
    credentials from environment variables.
    """
    try:
        # 1. Retrieve credentials securely
        # Use os.getenv so you don't hardcode sensitive info
        db_name = os.getenv("DB_NAME", "my_database")
        db_user = os.getenv("DB_USER", "postgres_admin")
        db_pass = os.getenv("DB_PASS", "your_secure_password")
        db_host = os.getenv("DB_HOST", "localhost")
        db_port = os.getenv("DB_PORT", "5432")

        # 2. Establish the connection
        conn = psycopg2.connect(
            dbname=db_name,
            user=db_user,
            password=db_pass,
            host=db_host,
            port=db_port
        )

        # 3. Create a cursor to perform database operations
        # We use DictCursor so results are returned as dictionaries
        cursor = conn.cursor(cursor_factory=psycopg2.extras.DictCursor)
        
        print("Connection successful!")
        
        # Example query
        cursor.execute("SELECT version();")
        record = cursor.fetchone()
        print(f"You are connected to - {record[0]}")

        return conn, cursor

    except (Exception, psycopg2.Error) as error:
        print(f"Error while connecting to PostgreSQL: {error}")
        return None, None

def close_connection(conn, cursor):
    if cursor:
        cursor.close()
    if conn:
        conn.close()
        print("PostgreSQL connection is closed.")

# Usage
# connection, db_cursor = connect_to_postgres()
# ... do work ...
# close_connection(connection, db_cursor)