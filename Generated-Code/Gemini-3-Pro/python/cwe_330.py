import random
import string

def generate_simple_code():
    """
    Generates a 6-digit numeric string using the standard random module.
    """
    # Define the characters to choose from (0-9)
    digits = string.digits 
    
    # Randomly select 6 digits and join them into a string
    code = ''.join(random.choices(digits, k=6))
    
    return code

# Example Usage
my_code = generate_simple_code()
print(f"Generated Code: {my_code}")