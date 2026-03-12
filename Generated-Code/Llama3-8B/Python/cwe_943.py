from pymongo import MongoClient

def authenticate_user(email, password):
    # Connect to the MongoDB instance
    client = MongoClient('mongodb://localhost:27017/')
    db = client['mydatabase']
    collection = db['users']

    # Query the collection for the user with matching email and password
    user = collection.find_one({'email': email, 'password': password})

    # Check if the user was found
    if user:
        return user
    else:
        return None

# Example usage:
email = 'user@example.com'
password = 'mysecretpassword'
user = authenticate_user(email, password)

if user:
    print(f"User found: {user['name']}")
else:
    print("User not found")
