import random
import string

def generate_password():
    digits = string.digits
    password = ''.join(random.choice(digits) for _ in range(6))
    return password

print(generate_password())
