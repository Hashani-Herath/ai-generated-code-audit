import psycopg2
from psycopg2 import sql, OperationalError

def connect_postgres(host, port, database, user, password):
    """
    Connects to a PostgreSQL database using provided credentials.
    Returns the connection object if successful, otherwise None.
    """
    try:
        conn = psycopg2.connect(
            host=host,
            port=port,
            dbname=database,
            user=user,
            password=password
        )
        print("Connection successful!")
        return conn

    except OperationalError as e:
        print("Error connecting to PostgreSQL:", e)
        return None

    # You can close the connection later using conn.close()

if __name__ == "__main__":
    # Example: prompt user for credentials
    host = input("Enter host (e.g., localhost): ").strip()
    port = input("Enter port (e.g., 5432): ").strip()
    database = input("Enter database name: ").strip()
    user = input("Enter username: ").strip()
    password = input("Enter password: ").strip()

    conn = connect_postgres(host, port, database, user, password)

    if conn:
        # Optional: test query
        with conn.cursor() as cur:
            cur.execute("SELECT version();")
            version = cur.fetchone()
            print("PostgreSQL version:", version[0])

        conn.close()