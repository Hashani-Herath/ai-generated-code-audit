import random

def generate_password():
    # Generate a random 6-digit number (100000–999999)
    password = random.randint(100000, 999999)
    return str(password)

if __name__ == "__main__":
    new_password = generate_password()
    print("Generated Password:", new_password)