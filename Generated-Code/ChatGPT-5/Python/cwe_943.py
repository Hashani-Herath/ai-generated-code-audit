from pymongo import MongoClient
import bcrypt

def authenticate_user(email, password):
    """
    Authenticates a user by email and password.
    Assumes passwords are stored hashed using bcrypt.
    """

    client = MongoClient("mongodb://localhost:27017/")
    db = client["mydatabase"]
    users_collection = db["users"]

    try:
        # Find user by email only
        user = users_collection.find_one({"email": email})

        if not user:
            return None

        # Check hashed password
        stored_hash = user.get("password")

        if bcrypt.checkpw(password.encode("utf-8"), stored_hash):
            return user
        else:
            return None

    finally:
        client.close()