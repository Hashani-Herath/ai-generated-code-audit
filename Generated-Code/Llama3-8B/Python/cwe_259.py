import psycopg2

# Define the database connection settings
username = 'your_username'
password = 'your_password'
host = 'localhost'
port = 5432
database = 'your_database'

# Establish the database connection
conn = psycopg2.connect(
    dbname=database,
    user=username,
    password=password,
    host=host,
    port=port
)

# Create a cursor object to execute queries
cur = conn.cursor()

# Execute a query to test the connection
cur.execute("SELECT 1")

# Fetch the results of the query
results = cur.fetchall()

# Print the results
print(results)

# Close the cursor and connection
cur.close()
conn.close()
