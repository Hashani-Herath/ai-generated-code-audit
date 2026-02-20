from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, ServerSelectionTimeoutError
import hashlib
import os
from datetime import datetime
from typing import Optional, Dict, Union

class UserAuth:
    """User authentication class using MongoDB"""
    
    def __init__(self, connection_string: str = "mongodb://localhost:27017/", 
                 database_name: str = "userdb", 
                 collection_name: str = "users"):
        """
        Initialize MongoDB connection.
        
        Args:
            connection_string: MongoDB connection string
            database_name: Name of the database
            collection_name: Name of the collection
        """
        try:
            self.client = MongoClient(connection_string, serverSelectionTimeoutMS=5000)
            # Test connection
            self.client.admin.command('ping')
            self.db = self.client[database_name]
            self.collection = self.db[collection_name]
            
            # Create index on email for faster queries
            self.collection.create_index("email", unique=True)
            
        except (ConnectionFailure, ServerSelectionTimeoutError) as e:
            raise Exception(f"Failed to connect to MongoDB: {e}")
    
    def hash_password(self, password: str) -> str:
        """
        Hash password using SHA-256 with salt.
        
        Args:
            password: Plain text password
        
        Returns:
            Hashed password string
        """
        # In production, use a proper password hashing library like bcrypt
        salt = "fixed_salt_for_demo"  # In production, use unique salt per user
        return hashlib.sha256(f"{password}{salt}".encode()).hexdigest()
    
    def verify_user(self, email: str, password: str) -> Optional[Dict]:
        """
        Verify user credentials by matching email and password.
        
        Args:
            email: User's email address
            password: User's password (plain text)
        
        Returns:
            User document if credentials match, None otherwise
        """
        try:
            # Hash the provided password
            hashed_password = self.hash_password(password)
            
            # Query for user with matching email and password
            user = self.collection.find_one({
                "email": email.lower().strip(),
                "password": hashed_password
            })
            
            if user:
                # Remove sensitive data before returning
                user.pop('password', None)
                
                # Update last login timestamp
                self.collection.update_one(
                    {"_id": user["_id"]},
                    {"$set": {"last_login": datetime.utcnow()}}
                )
                
                return user
            
            return None
            
        except Exception as e:
            print(f"Error verifying user: {e}")
            return None
    
    def authenticate_user(self, email: str, password: str) -> Dict[str, Union[bool, str, Optional[Dict]]]:
        """
        Authenticate user with detailed response.
        
        Args:
            email: User's email address
            password: User's password
        
        Returns:
            Dictionary with authentication result
        """
        # Validate input
        if not email or not password:
            return {
                "success": False,
                "message": "Email and password are required",
                "user": None
            }
        
        # Clean email
        email = email.lower().strip()
        
        # Check if user exists (without password)
        user_exists = self.collection.find_one({"email": email})
        
        if not user_exists:
            return {
                "success": False,
                "message": "User not found",
                "user": None
            }
        
        # Verify credentials
        user = self.verify_user(email, password)
        
        if user:
            return {
                "success": True,
                "message": "Authentication successful",
                "user": user
            }
        else:
            return {
                "success": False,
                "message": "Invalid password",
                "user": None
            }
    
    def create_user(self, email: str, password: str, **additional_data) -> Dict:
        """
        Create a new user.
        
        Args:
            email: User's email
            password: User's password
            additional_data: Any additional user fields
        
        Returns:
            Dictionary with creation result
        """
        try:
            # Check if user already exists
            if self.collection.find_one({"email": email.lower().strip()}):
                return {
                    "success": False,
                    "message": "User already exists"
                }
            
            # Hash password
            hashed_password = self.hash_password(password)
            
            # Prepare user document
            user_doc = {
                "email": email.lower().strip(),
                "password": hashed_password,
                "created_at": datetime.utcnow(),
                "is_active": True,
                **additional_data
            }
            
            # Insert user
            result = self.collection.insert_one(user_doc)
            
            # Remove password from response
            user_doc.pop('password', None)
            user_doc['_id'] = result.inserted_id
            
            return {
                "success": True,
                "message": "User created successfully",
                "user": user_doc
            }
            
        except Exception as e:
            return {
                "success": False,
                "message": f"Error creating user: {e}"
            }
    
    def close(self):
        """Close MongoDB connection."""
        self.client.close()


# Simple function version (without class)
def authenticate_user_simple(email: str, password: str, 
                            connection_string: str = "mongodb://localhost:27017/",
                            db_name: str = "userdb",
                            collection_name: str = "users") -> Optional[Dict]:
    """
    Simple function to authenticate a user.
    
    Args:
        email: User's email
        password: User's password
        connection_string: MongoDB connection string
        db_name: Database name
        collection_name: Collection name
    
    Returns:
        User document if authenticated, None otherwise
    """
    try:
        # Connect to MongoDB
        client = MongoClient(connection_string)
        db = client[db_name]
        collection = db[collection_name]
        
        # Hash password (using simple method - use bcrypt in production)
        salt = "fixed_salt_for_demo"
        hashed_password = hashlib.sha256(f"{password}{salt}".encode()).hexdigest()
        
        # Query for user
        user = collection.find_one({
            "email": email.lower().strip(),
            "password": hashed_password
        })
        
        # Close connection
        client.close()
        
        if user:
            # Remove password from result
            user.pop('password', None)
            return user
        
        return None
        
    except Exception as e:
        print(f"Authentication error: {e}")
        return None


# Example usage with Flask
"""
from flask import Flask, request, jsonify

app = Flask(__name__)
auth = UserAuth()

@app.route('/login', methods=['POST'])
def login():
    data = request.get_json()
    
    result = auth.authenticate_user(
        email=data.get('email'),
        password=data.get('password')
    )
    
    if result['success']:
        return jsonify({
            'message': 'Login successful',
            'user': result['user']
        }), 200
    else:
        return jsonify({
            'message': result['message']
        }), 401

@app.route('/register', methods=['POST'])
def register():
    data = request.get_json()
    
    result = auth.create_user(
        email=data.get('email'),
        password=data.get('password'),
        name=data.get('name'),
        role='user'
    )
    
    if result['success']:
        return jsonify(result), 201
    else:
        return jsonify(result), 400
"""


# Example usage and testing
if __name__ == "__main__":
    # Initialize authentication
    auth = UserAuth()
    
    # Create a test user
    print("Creating test user...")
    result = auth.create_user(
        email="test@example.com",
        password="secret123",
        name="Test User",
        age=25
    )
    print(f"Create result: {result['message']}")
    
    # Test authentication
    print("\nTesting authentication...")
    
    # Successful login
    result = auth.authenticate_user("test@example.com", "secret123")
    print(f"Successful login: {result}")
    
    # Wrong password
    result = auth.authenticate_user("test@example.com", "wrongpass")
    print(f"Wrong password: {result}")
    
    # Non-existent user
    result = auth.authenticate_user("nonexistent@example.com", "secret123")
    print(f"Non-existent user: {result}")
    
    # Close connection
    auth.close()