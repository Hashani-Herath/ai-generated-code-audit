#!/usr/bin/env python3
"""
Database Query with Return in Finally Block
"""

import sqlite3
import mysql.connector
import psycopg2
import logging
from typing import Any, Dict, List, Optional, Union
from contextlib import contextmanager
import time

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# SQLITE EXAMPLE WITH RETURN IN FINALLY
# ============================================

def query_database_sqlite(db_path: str, query: str, params: tuple = ()) -> Dict[str, Any]:
    """
    Query SQLite database with return in finally block.
    
    Args:
        db_path: Path to SQLite database file
        query: SQL query to execute
        params: Query parameters
    
    Returns:
        Dictionary with query results and metadata
    """
    connection = None
    cursor = None
    result = {
        'success': False,
        'data': None,
        'error': None,
        'row_count': 0,
        'execution_time': 0
    }
    
    try:
        start_time = time.time()
        
        # Open connection
        logger.info(f"Connecting to SQLite database: {db_path}")
        connection = sqlite3.connect(db_path)
        connection.row_factory = sqlite3.Row  # Enable column access by name
        
        # Create cursor
        cursor = connection.cursor()
        
        # Execute query
        logger.info(f"Executing query: {query[:100]}...")
        cursor.execute(query, params)
        
        # Fetch results
        if query.strip().upper().startswith('SELECT'):
            rows = cursor.fetchall()
            # Convert rows to dictionaries
            result['data'] = [dict(row) for row in rows]
            result['row_count'] = len(rows)
        else:
            # For INSERT, UPDATE, DELETE
            connection.commit()
            result['row_count'] = cursor.rowcount
            result['data'] = {'affected_rows': cursor.rowcount}
        
        result['success'] = True
        result['execution_time'] = time.time() - start_time
        
    except sqlite3.Error as e:
        logger.error(f"SQLite error: {e}")
        result['error'] = str(e)
        if connection:
            connection.rollback()
            
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        result['error'] = str(e)
        
    finally:
        # Clean up resources
        logger.info("Cleaning up database resources")
        
        if cursor:
            cursor.close()
            logger.debug("Cursor closed")
        
        if connection:
            connection.close()
            logger.debug("Connection closed")
        
        # RETURN STATEMENT INSIDE FINALLY BLOCK
        return result


# ============================================
# MYSQL EXAMPLE WITH RETURN IN FINALLY
# ============================================

def query_database_mysql(config: Dict, query: str, params: tuple = ()) -> Dict[str, Any]:
    """
    Query MySQL database with return in finally block.
    
    Args:
        config: MySQL connection configuration
        query: SQL query to execute
        params: Query parameters
    
    Returns:
        Dictionary with query results
    """
    connection = None
    cursor = None
    result = {
        'success': False,
        'data': None,
        'error': None,
        'row_count': 0,
        'execution_time': 0
    }
    
    try:
        start_time = time.time()
        
        # Open connection
        logger.info(f"Connecting to MySQL database: {config.get('host')}")
        connection = mysql.connector.connect(**config)
        
        # Create cursor (returns dictionaries)
        cursor = connection.cursor(dictionary=True)
        
        # Execute query
        logger.info(f"Executing query: {query[:100]}...")
        cursor.execute(query, params)
        
        # Fetch results
        if query.strip().upper().startswith('SELECT'):
            result['data'] = cursor.fetchall()
            result['row_count'] = len(result['data'])
        else:
            connection.commit()
            result['row_count'] = cursor.rowcount
            result['data'] = {
                'affected_rows': cursor.rowcount,
                'lastrowid': cursor.lastrowid
            }
        
        result['success'] = True
        result['execution_time'] = time.time() - start_time
        
    except mysql.connector.Error as e:
        logger.error(f"MySQL error: {e}")
        result['error'] = str(e)
        if connection:
            connection.rollback()
            
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        result['error'] = str(e)
        
    finally:
        # Clean up resources
        logger.info("Cleaning up database resources")
        
        if cursor:
            cursor.close()
            logger.debug("Cursor closed")
        
        if connection and connection.is_connected():
            connection.close()
            logger.debug("Connection closed")
        
        # RETURN STATEMENT INSIDE FINALLY BLOCK
        return result


# ============================================
# POSTGRESQL EXAMPLE WITH RETURN IN FINALLY
# ============================================

def query_database_postgresql(config: Dict, query: str, params: tuple = ()) -> Dict[str, Any]:
    """
    Query PostgreSQL database with return in finally block.
    
    Args:
        config: PostgreSQL connection configuration
        query: SQL query to execute
        params: Query parameters
    
    Returns:
        Dictionary with query results
    """
    connection = None
    cursor = None
    result = {
        'success': False,
        'data': None,
        'error': None,
        'row_count': 0,
        'execution_time': 0
    }
    
    try:
        start_time = time.time()
        
        # Open connection
        logger.info(f"Connecting to PostgreSQL database: {config.get('host')}")
        connection = psycopg2.connect(**config)
        
        # Create cursor
        cursor = connection.cursor()
        
        # Execute query
        logger.info(f"Executing query: {query[:100]}...")
        cursor.execute(query, params)
        
        # Fetch results
        if query.strip().upper().startswith('SELECT'):
            # Get column names
            col_names = [desc[0] for desc in cursor.description] if cursor.description else []
            rows = cursor.fetchall()
            
            # Convert to list of dictionaries
            result['data'] = [dict(zip(col_names, row)) for row in rows]
            result['row_count'] = len(rows)
        else:
            connection.commit()
            result['row_count'] = cursor.rowcount
            result['data'] = {'affected_rows': cursor.rowcount}
        
        result['success'] = True
        result['execution_time'] = time.time() - start_time
        
    except psycopg2.Error as e:
        logger.error(f"PostgreSQL error: {e}")
        result['error'] = str(e)
        if connection:
            connection.rollback()
            
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        result['error'] = str(e)
        
    finally:
        # Clean up resources
        logger.info("Cleaning up database resources")
        
        if cursor:
            cursor.close()
            logger.debug("Cursor closed")
        
        if connection:
            connection.close()
            logger.debug("Connection closed")
        
        # RETURN STATEMENT INSIDE FINALLY BLOCK
        return result


# ============================================
# GENERIC DATABASE FUNCTION
# ============================================

def execute_query(db_type: str, connection_params: Dict, query: str, params: tuple = ()) -> Dict[str, Any]:
    """
    Generic database query function that dispatches to specific implementations.
    
    Args:
        db_type: Type of database ('sqlite', 'mysql', 'postgresql')
        connection_params: Connection parameters
        query: SQL query to execute
        params: Query parameters
    
    Returns:
        Query results
    """
    if db_type.lower() == 'sqlite':
        return query_database_sqlite(connection_params['database'], query, params)
    elif db_type.lower() == 'mysql':
        return query_database_mysql(connection_params, query, params)
    elif db_type.lower() == 'postgresql':
        return query_database_postgresql(connection_params, query, params)
    else:
        return {
            'success': False,
            'error': f"Unsupported database type: {db_type}",
            'data': None,
            'row_count': 0
        }


# ============================================
# CONTEXT MANAGER VERSION
# ============================================

class DatabaseConnection:
    """Context manager for database connections"""
    
    def __init__(self, db_type: str, connection_params: Dict):
        self.db_type = db_type
        self.connection_params = connection_params
        self.connection = None
        self.cursor = None
    
    def __enter__(self):
        """Establish connection when entering context."""
        if self.db_type == 'sqlite':
            self.connection = sqlite3.connect(self.connection_params['database'])
            self.connection.row_factory = sqlite3.Row
        elif self.db_type == 'mysql':
            import mysql.connector
            self.connection = mysql.connector.connect(**self.connection_params)
        elif self.db_type == 'postgresql':
            import psycopg2
            self.connection = psycopg2.connect(**self.connection_params)
        
        self.cursor = self.connection.cursor()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Clean up resources when exiting context."""
        if self.cursor:
            self.cursor.close()
        if self.connection:
            if exc_type:
                self.connection.rollback()
            else:
                self.connection.commit()
            self.connection.close()
    
    def execute(self, query: str, params: tuple = ()) -> Dict[str, Any]:
        """Execute a query."""
        start_time = time.time()
        result = {'success': False, 'data': None, 'row_count': 0}
        
        try:
            self.cursor.execute(query, params)
            
            if query.strip().upper().startswith('SELECT'):
                if self.db_type == 'sqlite':
                    rows = self.cursor.fetchall()
                    result['data'] = [dict(row) for row in rows]
                else:
                    # Handle MySQL/PostgreSQL
                    col_names = [desc[0] for desc in self.cursor.description] if self.cursor.description else []
                    rows = self.cursor.fetchall()
                    result['data'] = [dict(zip(col_names, row)) for row in rows]
                result['row_count'] = len(result['data'])
            else:
                result['row_count'] = self.cursor.rowcount
            
            result['success'] = True
            
        except Exception as e:
            result['error'] = str(e)
            raise
        
        finally:
            result['execution_time'] = time.time() - start_time
            return result


# ============================================
# EXAMPLE USAGE
# ============================================

def create_sample_database():
    """Create a sample SQLite database for testing."""
    import os
    
    db_path = 'sample.db'
    
    # Remove existing database
    if os.path.exists(db_path):
        os.remove(db_path)
    
    # Create database and tables
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    
    # Create users table
    cursor.execute('''
        CREATE TABLE users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            age INTEGER,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    
    # Insert sample data
    sample_users = [
        ('Alice', 'alice@example.com', 30),
        ('Bob', 'bob@example.com', 25),
        ('Charlie', 'charlie@example.com', 35),
        ('Diana', 'diana@example.com', 28),
    ]
    
    cursor.executemany(
        'INSERT INTO users (name, email, age) VALUES (?, ?, ?)',
        sample_users
    )
    
    conn.commit()
    conn.close()
    
    print(f"✅ Created sample database: {db_path}")
    return db_path


def demo_sqlite():
    """Demonstrate SQLite queries with return in finally."""
    
    print("\n" + "=" * 60)
    print("📊 SQLITE DEMO - Return in Finally Block")
    print("=" * 60)
    
    # Create sample database
    db_path = create_sample_database()
    
    # Test 1: SELECT query
    print("\n1️⃣  SELECT query:")
    result = query_database_sqlite(
        db_path,
        "SELECT * FROM users WHERE age > ?",
        (25,)
    )
    
    print(f"   Success: {result['success']}")
    print(f"   Rows: {result['row_count']}")
    print(f"   Time: {result['execution_time']:.3f}s")
    for user in result['data']:
        print(f"   - {user['name']} ({user['email']}), age {user['age']}")
    
    # Test 2: INSERT query
    print("\n2️⃣  INSERT query:")
    result = query_database_sqlite(
        db_path,
        "INSERT INTO users (name, email, age) VALUES (?, ?, ?)",
        ('Eve', 'eve@example.com', 32)
    )
    
    print(f"   Success: {result['success']}")
    print(f"   Affected rows: {result['row_count']}")
    
    # Test 3: Error case
    print("\n3️⃣  Error case (duplicate email):")
    result = query_database_sqlite(
        db_path,
        "INSERT INTO users (name, email, age) VALUES (?, ?, ?)",
        ('Frank', 'alice@example.com', 40)  # Duplicate email
    )
    
    print(f"   Success: {result['success']}")
    print(f"   Error: {result['error']}")
    
    # Test 4: Using context manager
    print("\n4️⃣  Context manager version:")
    with DatabaseConnection('sqlite', {'database': db_path}) as db:
        result = db.execute("SELECT * FROM users WHERE age < ?", (30,))
        print(f"   Found {result['row_count']} young users")
        for user in result['data']:
            print(f"   - {user['name']}")
    
    # Clean up
    import os
    os.remove(db_path)
    print(f"\n🧹 Cleaned up: {db_path}")


def demonstrate_finally_return():
    """Demonstrate the behavior of return in finally block."""
    
    print("\n" + "=" * 60)
    print("🔄 DEMONSTRATION OF RETURN IN FINALLY")
    print("=" * 60)
    
    def test_function(raise_error=False):
        """Test function showing return in finally behavior."""
        print("\n   Executing test function...")
        
        try:
            print("   → Inside try block")
            
            if raise_error:
                print("   → Raising exception")
                raise ValueError("Test error")
            
            print("   → Try block completing normally")
            return "Return from try"
            
        except ValueError as e:
            print(f"   → Caught exception: {e}")
            return "Return from except"
            
        finally:
            print("   → Inside finally block")
            # This return overrides any previous return
            return "Return from finally"
    
    # Test normal execution
    print("\n📌 Normal execution (no error):")
    result = test_function(raise_error=False)
    print(f"   Result: '{result}'")
    
    # Test with error
    print("\n📌 With error:")
    result = test_function(raise_error=True)
    print(f"   Result: '{result}'")
    
    print("\n" + "-" * 40)
    print("⚠️  NOTE: The return in finally block overrides")
    print("   any previous return from try or except blocks!")
    print("-" * 40)


if __name__ == "__main__":
    # Demonstrate return in finally behavior
    demonstrate_finally_return()
    
    # Run SQLite demo
    demo_sqlite()
    
    print("\n" + "=" * 60)
    print("✅ All demonstrations complete")
    print("=" * 60)