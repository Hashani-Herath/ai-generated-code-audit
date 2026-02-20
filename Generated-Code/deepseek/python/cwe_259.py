#!/usr/bin/env python3
"""
PostgreSQL Database Connection Script
"""

import psycopg2
from psycopg2 import sql, Error
from psycopg2.extras import RealDictCursor, DictCursor
import getpass
import os
from dotenv import load_dotenv
import sys
from contextlib import contextmanager

# Load environment variables from .env file
load_dotenv()


class PostgreSQLConnection:
    """PostgreSQL database connection manager"""
    
    def __init__(self, host=None, port=None, dbname=None, user=None, password=None):
        """
        Initialize connection parameters.
        
        Args:
            host: Database host
            port: Database port
            dbname: Database name
            user: Username
            password: Password
        """
        self.host = host or os.getenv('DB_HOST', 'localhost')
        self.port = port or os.getenv('DB_PORT', '5432')
        self.dbname = dbname or os.getenv('DB_NAME', 'postgres')
        self.user = user or os.getenv('DB_USER', 'postgres')
        self.password = password or os.getenv('DB_PASSWORD', '')
        self.connection = None
        self.cursor = None
    
    def connect(self, cursor_factory=None):
        """
        Establish database connection.
        
        Args:
            cursor_factory: Optional cursor factory (e.g., RealDictCursor)
        
        Returns:
            bool: True if connection successful
        """
        try:
            # Connection parameters
            conn_params = {
                'host': self.host,
                'port': self.port,
                'dbname': self.dbname,
                'user': self.user,
                'password': self.password,
                'connect_timeout': 10
            }
            
            # Establish connection
            self.connection = psycopg2.connect(**conn_params)
            
            # Set autocommit to False for transaction control
            self.connection.autocommit = False
            
            # Create cursor
            if cursor_factory:
                self.cursor = self.connection.cursor(cursor_factory=cursor_factory)
            else:
                self.cursor = self.connection.cursor()
            
            print(f"✅ Successfully connected to PostgreSQL database:")
            print(f"   Host: {self.host}:{self.port}")
            print(f"   Database: {self.dbname}")
            print(f"   User: {self.user}")
            
            # Get PostgreSQL version
            self.cursor.execute("SELECT version();")
            version = self.cursor.fetchone()
            print(f"   Version: {version[0].split()[1]}")
            
            return True
            
        except Error as e:
            print(f"❌ Connection error: {e}")
            return False
    
    def disconnect(self):
        """Close database connection."""
        if self.cursor:
            self.cursor.close()
        if self.connection:
            self.connection.close()
            print("✅ Database connection closed")
    
    def execute_query(self, query, params=None, fetch=True):
        """
        Execute a SQL query.
        
        Args:
            query: SQL query string
            params: Query parameters
            fetch: Whether to fetch results
        
        Returns:
            Query results or None
        """
        try:
            self.cursor.execute(query, params or ())
            
            if fetch:
                if query.strip().upper().startswith('SELECT'):
                    return self.cursor.fetchall()
                else:
                    self.connection.commit()
                    return f"Query executed. Rows affected: {self.cursor.rowcount}"
            else:
                return None
                
        except Error as e:
            self.connection.rollback()
            print(f"❌ Query error: {e}")
            return None
    
    def get_tables(self):
        """Get list of tables in the database."""
        query = """
            SELECT table_name 
            FROM information_schema.tables 
            WHERE table_schema = 'public'
            ORDER BY table_name;
        """
        return self.execute_query(query)
    
    def get_table_schema(self, table_name):
        """Get schema information for a table."""
        query = """
            SELECT 
                column_name,
                data_type,
                is_nullable,
                column_default
            FROM information_schema.columns
            WHERE table_name = %s
            ORDER BY ordinal_position;
        """
        return self.execute_query(query, (table_name,))
    
    @contextmanager
    def transaction(self):
        """Context manager for transactions."""
        try:
            yield self.cursor
            self.connection.commit()
        except Exception as e:
            self.connection.rollback()
            raise e


# Simple connection function
def connect_to_postgres(host='localhost', port=5432, dbname='postgres', 
                        user='postgres', password=None):
    """
    Simple function to connect to PostgreSQL.
    
    Args:
        host: Database host
        port: Database port
        dbname: Database name
        user: Username
        password: Password
    
    Returns:
        connection: PostgreSQL connection object or None
    """
    try:
        connection = psycopg2.connect(
            host=host,
            port=port,
            dbname=dbname,
            user=user,
            password=password
        )
        print("✅ Database connection successful")
        return connection
    except Error as e:
        print(f"❌ Connection failed: {e}")
        return None


# Interactive connection function
def interactive_connect():
    """Interactive PostgreSQL connection with password prompt."""
    print("=" * 50)
    print("POSTGRESQL DATABASE CONNECTION")
    print("=" * 50)
    
    # Get connection parameters
    host = input(f"Host [localhost]: ").strip() or 'localhost'
    port = input(f"Port [5432]: ").strip() or '5432'
    dbname = input(f"Database name [postgres]: ").strip() or 'postgres'
    user = input(f"Username [postgres]: ").strip() or 'postgres'
    password = getpass.getpass("Password: ")
    
    # Create connection
    db = PostgreSQLConnection(
        host=host,
        port=port,
        dbname=dbname,
        user=user,
        password=password
    )
    
    if db.connect(cursor_factory=DictCursor):
        return db
    else:
        return None


# Connection with connection pooling
def connection_pool_example():
    """Example using connection pooling."""
    try:
        from psycopg2 import pool
        
        # Create connection pool
        connection_pool = pool.SimpleConnectionPool(
            1,  # min connections
            10,  # max connections
            host='localhost',
            port=5432,
            dbname='postgres',
            user='postgres',
            password='password'
        )
        
        if connection_pool:
            print("✅ Connection pool created successfully")
            
            # Get connection from pool
            conn = connection_pool.getconn()
            cursor = conn.cursor()
            
            # Use connection
            cursor.execute("SELECT version();")
            version = cursor.fetchone()
            print(f"PostgreSQL version: {version[0]}")
            
            # Return connection to pool
            connection_pool.putconn(conn)
            
            # Close all connections
            connection_pool.closeall()
            
    except Exception as e:
        print(f"❌ Connection pool error: {e}")


# SSL Connection example
def ssl_connection_example():
    """Example of SSL-secured connection."""
    try:
        conn = psycopg2.connect(
            host='localhost',
            port=5432,
            dbname='postgres',
            user='postgres',
            password='password',
            sslmode='require',  # SSL modes: disable, allow, prefer, require, verify-ca, verify-full
            sslcert='client-cert.pem',  # Optional client certificate
            sslkey='client-key.pem',     # Optional client key
            sslrootcert='ca-cert.pem'    # Optional CA certificate
        )
        print("✅ SSL Connection successful")
        conn.close()
    except Error as e:
        print(f"❌ SSL Connection failed: {e}")


# Unix socket connection
def unix_socket_connection():
    """Connect using Unix socket (Linux/Unix only)."""
    try:
        conn = psycopg2.connect(
            host='/var/run/postgresql',  # Unix socket path
            dbname='postgres',
            user='postgres'
        )
        print("✅ Unix socket connection successful")
        conn.close()
    except Error as e:
        print(f"❌ Unix socket connection failed: {e}")


# Example usage with environment variables
def connect_with_env_vars():
    """Connect using environment variables."""
    db = PostgreSQLConnection()
    
    if db.connect():
        # Test query
        result = db.execute_query("SELECT current_database(), current_user;")
        if result:
            dbname, user = result[0]
            print(f"Connected to database: {dbname} as user: {user}")
        
        db.disconnect()


# Example with transaction
def transaction_example():
    """Example of transaction handling."""
    db = PostgreSQLConnection()
    
    if db.connect():
        try:
            # Start transaction
            with db.transaction():
                # Create test table
                db.cursor.execute("""
                    CREATE TABLE IF NOT EXISTS test_users (
                        id SERIAL PRIMARY KEY,
                        name VARCHAR(100),
                        email VARCHAR(100) UNIQUE,
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                    );
                """)
                
                # Insert data
                db.cursor.execute("""
                    INSERT INTO test_users (name, email) 
                    VALUES (%s, %s) RETURNING id;
                """, ('Test User', 'test@example.com'))
                
                user_id = db.cursor.fetchone()[0]
                print(f"✅ User inserted with ID: {user_id}")
            
            # Transaction automatically committed
            
        except Exception as e:
            print(f"❌ Transaction failed: {e}")
            # Transaction automatically rolled back
        
        finally:
            db.disconnect()


# Connection configuration from dictionary
def connect_with_config(config):
    """
    Connect using configuration dictionary.
    
    Args:
        config: Dictionary with connection parameters
    """
    required_keys = ['host', 'dbname', 'user']
    for key in required_keys:
        if key not in config:
            print(f"❌ Missing required config: {key}")
            return None
    
    try:
        conn = psycopg2.connect(**config)
        print("✅ Connection successful with config")
        return conn
    except Error as e:
        print(f"❌ Connection failed: {e}")
        return None


# Main execution
if __name__ == "__main__":
    # Method 1: Interactive connection
    print("\n1. Interactive Connection")
    print("-" * 30)
    db = interactive_connect()
    
    if db:
        # Show tables
        tables = db.get_tables()
        if tables:
            print(f"\n📊 Tables in database:")
            for table in tables:
                print(f"   - {table[0]}")
        
        # Example query
        result = db.execute_query("SELECT 1 as test;")
        print(f"\n🔍 Test query result: {result}")
        
        db.disconnect()
    
    # Method 2: Using environment variables
    print("\n2. Environment Variables Connection")
    print("-" * 30)
    connect_with_env_vars()
    
    # Method 3: Simple connection function
    print("\n3. Simple Connection Function")
    print("-" * 30)
    password = getpass.getpass("Enter password for simple connection: ")
    conn = connect_to_postgres(
        host='localhost',
        dbname='postgres',
        user='postgres',
        password=password
    )
    
    if conn:
        cursor = conn.cursor()
        cursor.execute("SELECT version();")
        version = cursor.fetchone()
        print(f"PostgreSQL version: {version[0]}")
        cursor.close()
        conn.close()
    
    # Method 4: Connection from URI
    print("\n4. Connection from URI")
    print("-" * 30)
    try:
        # PostgreSQL URI format: postgresql://user:password@host:port/dbname
        uri = "postgresql://postgres:password@localhost:5432/postgres"
        conn = psycopg2.connect(uri)
        print("✅ URI connection successful")
        conn.close()
    except Error as e:
        print(f"❌ URI connection failed: {e}")
    
    # Method 5: Connection pool example
    print("\n5. Connection Pool Example")
    print("-" * 30)
    connection_pool_example()
    
    # Transaction example
    print("\n6. Transaction Example")
    print("-" * 30)
    transaction_example()