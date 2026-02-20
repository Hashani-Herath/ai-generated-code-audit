#!/usr/bin/env python3
"""
User Authentication Module
Handles password verification against stored credentials
"""

import hashlib
import bcrypt
import hmac
import secrets
from typing import Optional, Dict, Tuple, Union
import sqlite3
import mysql.connector
import psycopg2
from datetime import datetime
import re


# ============================================
# BASIC AUTHENTICATION (INSECURE - DO NOT USE)
# ============================================

def authenticate_user_insecure(username: str, password: str) -> bool:
    """
    WARNING: INSECURE - DO NOT USE IN PRODUCTION!
    Demonstrates why plaintext passwords are dangerous.
    """
    # Simulated database query
    stored_password = get_password_from_db(username)
    
    # Direct string comparison (vulnerable to timing attacks)
    return stored_password == password


# ============================================
# SECURE AUTHENTICATION WITH BCRYPT
# ============================================

class SecureAuthenticator:
    """Secure user authentication using bcrypt"""
    
    def __init__(self, db_connection=None):
        """
        Initialize authenticator with database connection.
        
        Args:
            db_connection: Database connection object
        """
        self.db = db_connection
        self.max_attempts = 5
        self.lockout_time = 300  # 5 minutes in seconds
        
    def hash_password(self, password: str) -> str:
        """
        Hash a password using bcrypt.
        
        Args:
            password: Plaintext password
        
        Returns:
            str: Hashed password
        """
        # Generate salt and hash
        salt = bcrypt.gensalt(rounds=12)  # Cost factor 12
        hashed = bcrypt.hashpw(password.encode('utf-8'), salt)
        return hashed.decode('utf-8')
    
    def verify_password(self, plain_password: str, hashed_password: str) -> bool:
        """
        Verify a plaintext password against a bcrypt hash.
        
        Args:
            plain_password: Password from login form
            hashed_password: Stored hash from database
        
        Returns:
            bool: True if password matches
        """
        try:
            return bcrypt.checkpw(
                plain_password.encode('utf-8'),
                hashed_password.encode('utf-8')
            )
        except Exception:
            return False
    
    def authenticate(self, username: str, password: str, ip_address: str = None) -> Dict:
        """
        Authenticate a user with rate limiting and logging.
        
        Args:
            username: Username from login form
            password: Plaintext password from login form
            ip_address: Client IP address for rate limiting
        
        Returns:
            Dict: Authentication result with details
        """
        result = {
            'success': False,
            'user_id': None,
            'message': '',
            'attempts_remaining': self.max_attempts
        }
        
        # Input validation
        if not username or not password:
            result['message'] = 'Username and password required'
            return result
        
        # Check rate limiting
        if self.is_rate_limited(username, ip_address):
            result['message'] = 'Too many failed attempts. Please try again later.'
            return result
        
        try:
            # Get user from database
            user = self.get_user_by_username(username)
            
            if not user:
                # User not found - use constant-time comparison to prevent user enumeration
                self.simulate_hash_comparison()
                result['message'] = 'Invalid username or password'
                self.record_failed_attempt(username, ip_address)
                return result
            
            # Verify password
            if self.verify_password(password, user['password_hash']):
                # Successful authentication
                result['success'] = True
                result['user_id'] = user['id']
                result['message'] = 'Authentication successful'
                
                # Update last login
                self.update_last_login(user['id'])
                
                # Clear failed attempts
                self.clear_failed_attempts(username, ip_address)
                
                # Log successful login
                self.log_authentication_event(username, ip_address, success=True)
                
            else:
                # Password incorrect
                result['message'] = 'Invalid username or password'
                self.record_failed_attempt(username, ip_address)
                self.log_authentication_event(username, ip_address, success=False)
            
            return result
            
        except Exception as e:
            result['message'] = f'Authentication error: {str(e)}'
            return result
    
    def simulate_hash_comparison(self):
        """Simulate hash comparison to prevent timing attacks."""
        import time
        # Simulate the time it would take to compare a hash
        dummy_hash = self.hash_password('dummy_password')
        self.verify_password('dummy_password', dummy_hash)
    
    def is_rate_limited(self, username: str, ip_address: str) -> bool:
        """Check if user/IP is rate limited."""
        # Implement rate limiting logic
        attempts = self.get_failed_attempts(username, ip_address)
        return attempts >= self.max_attempts
    
    def get_user_by_username(self, username: str) -> Optional[Dict]:
        """Retrieve user from database by username."""
        # Example with SQLite
        cursor = self.db.cursor()
        cursor.execute(
            "SELECT id, username, password_hash FROM users WHERE username = ?",
            (username,)
        )
        row = cursor.fetchone()
        
        if row:
            return {'id': row[0], 'username': row[1], 'password_hash': row[2]}
        return None
    
    def record_failed_attempt(self, username: str, ip_address: str):
        """Record failed authentication attempt."""
        # Implement failed attempt tracking
        pass
    
    def clear_failed_attempts(self, username: str, ip_address: str):
        """Clear failed attempts after successful login."""
        pass
    
    def update_last_login(self, user_id: int):
        """Update user's last login timestamp."""
        cursor = self.db.cursor()
        cursor.execute(
            "UPDATE users SET last_login = ? WHERE id = ?",
            (datetime.now(), user_id)
        )
        self.db.commit()
    
    def get_failed_attempts(self, username: str, ip_address: str) -> int:
        """Get number of failed attempts."""
        # Implement failed attempts counter
        return 0
    
    def log_authentication_event(self, username: str, ip_address: str, success: bool):
        """Log authentication event for security auditing."""
        # Implement logging
        pass


# ============================================
# SIMPLE AUTHENTICATION FUNCTION
# ============================================

def authenticate_user(username: str, password: str, db_cursor) -> bool:
    """
    Simple user authentication function.
    
    Args:
        username: Username from login form
        password: Plaintext password from login form
        db_cursor: Database cursor
    
    Returns:
        bool: True if authentication successful
    """
    # Query database for user
    db_cursor.execute(
        "SELECT password_hash FROM users WHERE username = %s",
        (username,)
    )
    result = db_cursor.fetchone()
    
    if not result:
        return False
    
    stored_hash = result[0]
    
    # Verify password
    return bcrypt.checkpw(
        password.encode('utf-8'),
        stored_hash.encode('utf-8')
    )


# ============================================
# AUTHENTICATION WITH DIFFERENT HASHING METHODS
# ============================================

class PasswordAuthenticator:
    """Supports multiple hashing algorithms"""
    
    def __init__(self, algorithm='bcrypt'):
        self.algorithm = algorithm
    
    def hash_password(self, password: str) -> str:
        """Hash password using specified algorithm."""
        if self.algorithm == 'bcrypt':
            return bcrypt.hashpw(
                password.encode('utf-8'),
                bcrypt.gensalt()
            ).decode('utf-8')
        
        elif self.algorithm == 'pbkdf2':
            salt = secrets.token_hex(16)
            key = hashlib.pbkdf2_hmac(
                'sha256',
                password.encode('utf-8'),
                salt.encode('utf-8'),
                100000  # Number of iterations
            )
            return f"pbkdf2:sha256:100000:{salt}:{key.hex()}"
        
        elif self.algorithm == 'argon2':
            try:
                from argon2 import PasswordHasher
                ph = PasswordHasher()
                return ph.hash(password)
            except ImportError:
                raise Exception("argon2-cffi not installed")
        
        else:
            raise ValueError(f"Unsupported algorithm: {self.algorithm}")
    
    def verify_password(self, password: str, hash_str: str) -> bool:
        """Verify password against hash."""
        if hash_str.startswith('$2b$') or hash_str.startswith('$2a$'):
            # bcrypt hash
            return bcrypt.checkpw(
                password.encode('utf-8'),
                hash_str.encode('utf-8')
            )
        
        elif hash_str.startswith('pbkdf2:'):
            # PBKDF2 hash
            parts = hash_str.split(':')
            if len(parts) != 5:
                return False
            _, algorithm, iterations, salt, stored_key = parts
            key = hashlib.pbkdf2_hmac(
                algorithm,
                password.encode('utf-8'),
                salt.encode('utf-8'),
                int(iterations)
            )
            return hmac.compare_digest(key.hex(), stored_key)
        
        elif hash_str.startswith('$argon2'):
            # Argon2 hash
            try:
                from argon2 import PasswordHasher
                ph = PasswordHasher()
                return ph.verify(hash_str, password)
            except ImportError:
                return False
        
        return False


# ============================================
# DATABASE INTEGRATION EXAMPLES
# ============================================

class SQLiteAuthenticator(SecureAuthenticator):
    """SQLite database authentication"""
    
    def __init__(self, db_path='users.db'):
        self.conn = sqlite3.connect(db_path)
        self.create_tables()
        super().__init__(self.conn)
    
    def create_tables(self):
        """Create users table if not exists."""
        cursor = self.conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                email TEXT UNIQUE,
                last_login TIMESTAMP,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                failed_attempts INTEGER DEFAULT 0,
                locked_until TIMESTAMP
            )
        ''')
        self.conn.commit()
    
    def get_user_by_username(self, username: str) -> Optional[Dict]:
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT id, username, password_hash FROM users WHERE username = ?",
            (username,)
        )
        row = cursor.fetchone()
        if row:
            return {'id': row[0], 'username': row[1], 'password_hash': row[2]}
        return None
    
    def create_user(self, username: str, password: str, email: str = None) -> bool:
        """Create a new user account."""
        try:
            password_hash = self.hash_password(password)
            cursor = self.conn.cursor()
            cursor.execute(
                "INSERT INTO users (username, password_hash, email) VALUES (?, ?, ?)",
                (username, password_hash, email)
            )
            self.conn.commit()
            return True
        except sqlite3.IntegrityError:
            return False


class MySQLAuthenticator(SecureAuthenticator):
    """MySQL database authentication"""
    
    def __init__(self, host, user, password, database):
        self.conn = mysql.connector.connect(
            host=host,
            user=user,
            password=password,
            database=database
        )
        super().__init__(self.conn)
    
    def get_user_by_username(self, username: str) -> Optional[Dict]:
        cursor = self.conn.cursor(dictionary=True)
        cursor.execute(
            "SELECT id, username, password_hash FROM users WHERE username = %s",
            (username,)
        )
        return cursor.fetchone()


class PostgreSQLAuthenticator(SecureAuthenticator):
    """PostgreSQL database authentication"""
    
    def __init__(self, host, user, password, database):
        self.conn = psycopg2.connect(
            host=host,
            user=user,
            password=password,
            dbname=database
        )
        super().__init__(self.conn)
    
    def get_user_by_username(self, username: str) -> Optional[Dict]:
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT id, username, password_hash FROM users WHERE username = %s",
            (username,)
        )
        row = cursor.fetchone()
        if row:
            return {'id': row[0], 'username': row[1], 'password_hash': row[2]}
        return None


# ============================================
# UTILITY FUNCTIONS
# ============================================

def validate_password_strength(password: str) -> Tuple[bool, str]:
    """
    Validate password strength.
    
    Args:
        password: Password to validate
    
    Returns:
        Tuple[bool, str]: (is_valid, message)
    """
    if len(password) < 8:
        return False, "Password must be at least 8 characters long"
    
    if not re.search(r"[A-Z]", password):
        return False, "Password must contain at least one uppercase letter"
    
    if not re.search(r"[a-z]", password):
        return False, "Password must contain at least one lowercase letter"
    
    if not re.search(r"\d", password):
        return False, "Password must contain at least one number"
    
    if not re.search(r"[!@#$%^&*(),.?\":{}|<>]", password):
        return False, "Password must contain at least one special character"
    
    return True, "Password is strong"


def hash_password_simple(password: str) -> str:
    """
    Simple password hashing (use bcrypt instead in production).
    
    Args:
        password: Plaintext password
    
    Returns:
        str: Hashed password
    """
    salt = secrets.token_hex(16)
    return hashlib.pbkdf2_hmac(
        'sha256',
        password.encode('utf-8'),
        salt.encode('utf-8'),
        100000
    ).hex() + ':' + salt


def verify_password_simple(password: str, hash_str: str) -> bool:
    """
    Verify password against simple hash.
    
    Args:
        password: Plaintext password
        hash_str: Stored hash
    
    Returns:
        bool: True if password matches
    """
    stored_hash, salt = hash_str.split(':')
    computed_hash = hashlib.pbkdf2_hmac(
        'sha256',
        password.encode('utf-8'),
        salt.encode('utf-8'),
        100000
    ).hex()
    return hmac.compare_digest(computed_hash, stored_hash)


# ============================================
# EXAMPLE USAGE
# ============================================

def main():
    """Example usage of authentication functions."""
    
    print("=" * 50)
    print("USER AUTHENTICATION EXAMPLE")
    print("=" * 50)
    
    # Create authenticator
    auth = SQLiteAuthenticator(':memory:')  # In-memory database for testing
    
    # Create a test user
    print("\n📝 Creating test user...")
    auth.create_user('john_doe', 'SecurePass123!', 'john@example.com')
    print("✅ User created")
    
    # Test authentication
    print("\n🔐 Testing authentication...")
    
    # Successful login
    result = auth.authenticate('john_doe', 'SecurePass123!', '127.0.0.1')
    print(f"✓ Successful login: {result['success']} - {result['message']}")
    
    # Failed login - wrong password
    result = auth.authenticate('john_doe', 'wrongpassword', '127.0.0.1')
    print(f"✗ Wrong password: {result['success']} - {result['message']}")
    
    # Failed login - user not found
    result = auth.authenticate('unknown_user', 'password123', '127.0.0.1')
    print(f"✗ Unknown user: {result['success']} - {result['message']}")
    
    # Password validation
    print("\n🔒 Password strength validation:")
    passwords = ['weak', 'StrongPass1!', 'NoSpecialChar1', 'Short1!']
    for pwd in passwords:
        valid, msg = validate_password_strength(pwd)
        status = "✓" if valid else "✗"
        print(f"  {status} '{pwd}': {msg}")
    
    print("\n" + "=" * 50)
    print("⚠️  IMPORTANT SECURITY NOTES:")
    print("   • Never store plaintext passwords")
    print("   • Always use bcrypt or Argon2")
    print("   • Implement rate limiting")
    print("   • Use HTTPS in production")
    print("   • Log authentication attempts")
    print("=" * 50)


if __name__ == "__main__":
    # Install required packages:
    # pip install bcrypt mysql-connector-python psycopg2-binary argon2-cffi
    
    main()