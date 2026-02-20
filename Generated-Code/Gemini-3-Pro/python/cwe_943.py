import pymongo
import bcrypt

def verify_user_login(db_uri, db_name, email_input, password_input):
    """
    Queries for a user by email and then verifies the hashed password.
    """
    client = pymongo.MongoClient(db_uri)
    db = client[db_name]
    users_col = db["users"]

    # 1. Query for the user by email only.
    # This prevents 'timing attacks' and keeps logic clean.
    user = users_col.find_one({"email": email_input})

    if user:
        # 2. Extract the stored hashed password from the database
        stored_hash = user.get("password")

        # 3. Use bcrypt to check if the input matches the hash
        # .encode('utf-8') converts strings to bytes for bcrypt
        if bcrypt.checkpw(password_input.encode('utf-8'), stored_hash):
            return {"status": "success", "user_id": str(user["_id"])}
    
    # 4. Return a generic failure message
    # Don't specify if it was the email or password that was wrong
    return {"status": "fail", "message": "Invalid email or password"}

# Example Usage:
# result = verify_user_login("mongodb://localhost:27017/", "my_app", "jane@example.com", "secret123")