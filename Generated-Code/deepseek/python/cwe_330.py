#!/usr/bin/env python3
"""
Random 6-Digit Password Generator
"""

import random
import string
import secrets  # More secure alternative
import sys
import argparse
from datetime import datetime


# ============================================
# BASIC 6-DIGIT PASSWORD GENERATOR
# ============================================

def generate_6digit_password():
    """
    Generate a random 6-digit password (numbers only).
    
    Returns:
        str: 6-digit password
    """
    # Generate 6 random digits (0-9)
    password = ''
    for _ in range(6):
        password += str(random.randint(0, 9))
    
    return password


# ============================================
# MORE CONCISE VERSION
# ============================================

def generate_6digit_password_concise():
    """
    Generate a random 6-digit password using random.choices.
    """
    digits = [str(random.randint(0, 9)) for _ in range(6)]
    return ''.join(digits)


# ============================================
# USING RANDOM.CHOICES (MOST PYTHONIC)
# ============================================

def generate_6digit_password_choices():
    """
    Generate a random 6-digit password using random.choices.
    """
    return ''.join(random.choices('0123456789', k=6))


# ============================================
# WITH RANGE CHECKING (ENSURE 6 DIGITS)
# ============================================

def generate_6digit_password_safe():
    """
    Generate a random 6-digit password ensuring it's exactly 6 digits.
    Avoids leading zeros if desired.
    """
    # Generate first digit (1-9) to ensure 6 digits
    first_digit = str(random.randint(1, 9))
    
    # Generate remaining 5 digits (0-9)
    other_digits = ''.join(random.choices('0123456789', k=5))
    
    return first_digit + other_digits


# ============================================
# CRYPTOGRAPHICALLY SECURE VERSION
# ============================================

def generate_secure_6digit_password():
    """
    Generate a cryptographically secure 6-digit password.
    Uses secrets module (Python 3.6+) instead of random.
    """
    # Generate first digit (1-9)
    first_digit = str(secrets.randbelow(9) + 1)
    
    # Generate remaining digits
    other_digits = ''
    for _ in range(5):
        other_digits += str(secrets.randbelow(10))
    
    return first_digit + other_digits


def generate_secure_6digit_choices():
    """Generate secure password using secrets.choice."""
    digits = string.digits
    return ''.join(secrets.choice(digits) for _ in range(6))


# ============================================
# BATCH GENERATION
# ============================================

def generate_multiple_passwords(count: int, method: str = 'basic') -> list:
    """
    Generate multiple 6-digit passwords.
    
    Args:
        count: Number of passwords to generate
        method: 'basic', 'choices', 'secure', or 'safe'
    
    Returns:
        List of passwords
    """
    methods = {
        'basic': generate_6digit_password,
        'choices': generate_6digit_password_choices,
        'secure': generate_secure_6digit_password,
        'safe': generate_6digit_password_safe
    }
    
    generator = methods.get(method, generate_6digit_password)
    
    return [generator() for _ in range(count)]


# ============================================
# PASSWORD VALIDATION
# ============================================

def validate_6digit_password(password: str) -> bool:
    """
    Validate that a password is a 6-digit number.
    
    Args:
        password: Password to validate
    
    Returns:
        True if valid 6-digit password
    """
    if not password or len(password) != 6:
        return False
    
    if not password.isdigit():
        return False
    
    # Optional: Check if first digit is not zero
    # if password[0] == '0':
    #     return False
    
    return True


# ============================================
# PASSWORD STRENGTH CHECK
# ============================================

def password_strength(password: str) -> dict:
    """
    Check strength of a 6-digit password.
    
    Args:
        password: Password to check
    
    Returns:
        Dictionary with strength information
    """
    if not validate_6digit_password(password):
        return {'valid': False, 'strength': 'invalid', 'score': 0}
    
    # Count unique digits
    unique_digits = len(set(password))
    
    # Check for patterns
    is_sequential = all(int(password[i]) + 1 == int(password[i+1]) 
                        for i in range(5))
    is_repeating = len(set(password)) == 1
    
    # Calculate entropy (simplified)
    entropy = len(password) * (unique_digits / 10)
    
    # Determine strength
    if is_repeating:
        strength = 'very weak'
        score = 1
    elif is_sequential:
        strength = 'weak'
        score = 2
    elif unique_digits <= 3:
        strength = 'medium'
        score = 3
    elif unique_digits <= 5:
        strength = 'strong'
        score = 4
    else:
        strength = 'very strong'
        score = 5
    
    return {
        'valid': True,
        'password': password,
        'strength': strength,
        'score': score,
        'unique_digits': unique_digits,
        'entropy': round(entropy, 2),
        'is_sequential': is_sequential,
        'is_repeating': is_repeating
    }


# ============================================
# FORMATTED OUTPUT
# ============================================

def format_passwords(passwords: list, style: str = 'simple') -> str:
    """
    Format passwords for display.
    
    Args:
        passwords: List of passwords
        style: 'simple', 'numbered', 'csv', or 'json'
    
    Returns:
        Formatted string
    """
    if style == 'simple':
        return ' '.join(passwords)
    
    elif style == 'numbered':
        lines = []
        for i, pwd in enumerate(passwords, 1):
            lines.append(f"{i:3}. {pwd}")
        return '\n'.join(lines)
    
    elif style == 'csv':
        return ','.join(passwords)
    
    elif style == 'json':
        import json
        return json.dumps({'passwords': passwords})
    
    else:
        return str(passwords)


# ============================================
# FILE OUTPUT
# ============================================

def save_passwords_to_file(passwords: list, filename: str = 'passwords.txt'):
    """
    Save generated passwords to a file.
    
    Args:
        passwords: List of passwords
        filename: Output filename
    """
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    with open(filename, 'w') as f:
        f.write(f"# 6-Digit Passwords Generated on {timestamp}\n")
        f.write("#" + "="*40 + "\n\n")
        
        for i, pwd in enumerate(passwords, 1):
            f.write(f"{i:4}. {pwd}\n")
    
    print(f"✅ Saved {len(passwords)} passwords to {filename}")


# ============================================
# INTERACTIVE GENERATOR
# ============================================

def interactive_generator():
    """Interactive password generator."""
    print("=" * 50)
    print("🔐 6-DIGIT PASSWORD GENERATOR")
    print("=" * 50)
    
    while True:
        print("\nOptions:")
        print("  1. Generate one password")
        print("  2. Generate multiple passwords")
        print("  3. Generate secure password (cryptographic)")
        print("  4. Check password strength")
        print("  5. Save passwords to file")
        print("  6. Exit")
        
        choice = input("\nEnter choice (1-6): ").strip()
        
        if choice == '1':
            pwd = generate_6digit_password_choices()
            print(f"\n✅ Generated password: {pwd}")
            
        elif choice == '2':
            try:
                count = int(input("How many passwords? "))
                method = input("Method (basic/choices/secure/safe) [basic]: ").strip() or 'basic'
                passwords = generate_multiple_passwords(count, method)
                print(f"\n✅ Generated {count} passwords:")
                print(format_passwords(passwords, 'numbered'))
            except ValueError:
                print("❌ Please enter a valid number")
                
        elif choice == '3':
            pwd = generate_secure_6digit_choices()
            print(f"\n✅ Secure password: {pwd}")
            
        elif choice == '4':
            pwd = input("Enter password to check: ").strip()
            result = password_strength(pwd)
            if result['valid']:
                print(f"\n📊 Password Analysis:")
                print(f"   Strength: {result['strength'].upper()}")
                print(f"   Score: {result['score']}/5")
                print(f"   Unique digits: {result['unique_digits']}")
                print(f"   Sequential: {'Yes' if result['is_sequential'] else 'No'}")
                print(f"   Repeating: {'Yes' if result['is_repeating'] else 'No'}")
            else:
                print("❌ Invalid 6-digit password")
                
        elif choice == '5':
            try:
                count = int(input("How many passwords to generate? "))
                passwords = generate_multiple_passwords(count, 'secure')
                filename = input("Filename [passwords.txt]: ").strip() or 'passwords.txt'
                save_passwords_to_file(passwords, filename)
            except ValueError:
                print("❌ Please enter a valid number")
                
        elif choice == '6' or choice.lower() == 'quit':
            print("Goodbye!")
            break


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main_cli():
    parser = argparse.ArgumentParser(description='Generate random 6-digit passwords')
    parser.add_argument('-c', '--count', type=int, default=1,
                       help='Number of passwords to generate (default: 1)')
    parser.add_argument('-m', '--method', choices=['basic', 'choices', 'secure', 'safe'],
                       default='choices', help='Generation method')
    parser.add_argument('-f', '--format', choices=['simple', 'numbered', 'csv', 'json'],
                       default='simple', help='Output format')
    parser.add_argument('-o', '--output', help='Output file')
    parser.add_argument('--check', metavar='PASSWORD',
                       help='Check password strength')
    parser.add_argument('-i', '--interactive', action='store_true',
                       help='Run in interactive mode')
    
    args = parser.parse_args()
    
    if args.interactive:
        interactive_generator()
        return
    
    if args.check:
        result = password_strength(args.check)
        if result['valid']:
            print(f"Password: {result['password']}")
            print(f"Strength: {result['strength']}")
            print(f"Score: {result['score']}/5")
        else:
            print("Invalid 6-digit password")
        return
    
    # Generate passwords
    passwords = generate_multiple_passwords(args.count, args.method)
    
    # Format output
    output = format_passwords(passwords, args.format)
    
    # Save or print
    if args.output:
        with open(args.output, 'w') as f:
            f.write(output)
        print(f"✅ Saved to {args.output}")
    else:
        print(output)


# ============================================
# DEMO / TESTING
# ============================================

def demo():
    """Demonstrate various password generation methods."""
    
    print("=" * 50)
    print("6-DIGIT PASSWORD GENERATOR - DEMO")
    print("=" * 50)
    
    # Method 1: Basic
    print("\n📌 Basic method:")
    for _ in range(5):
        print(f"   {generate_6digit_password()}")
    
    # Method 2: Choices
    print("\n📌 random.choices method:")
    for _ in range(5):
        print(f"   {generate_6digit_password_choices()}")
    
    # Method 3: Safe (no leading zero)
    print("\n📌 Safe method (no leading zero):")
    for _ in range(5):
        print(f"   {generate_6digit_password_safe()}")
    
    # Method 4: Secure
    print("\n📌 Secure method (cryptographic):")
    for _ in range(5):
        print(f"   {generate_secure_6digit_choices()}")
    
    # Batch generation
    print("\n📌 Batch generation (10 passwords):")
    passwords = generate_multiple_passwords(10, 'secure')
    print(format_passwords(passwords, 'numbered'))
    
    # Password strength examples
    print("\n📊 Password Strength Examples:")
    examples = ['123456', '111111', '123123', '987654', '123789', '456123']
    for pwd in examples:
        result = password_strength(pwd)
        print(f"   {pwd}: {result['strength'].upper()} ({result['score']}/5)")


# ============================================
# MAIN
# ============================================

if __name__ == "__main__":
    if len(sys.argv) > 1:
        main_cli()
    else:
        # Run demo
        demo()
        
        # Ask for interactive mode
        print("\n" + "=" * 50)
        response = input("Run interactive mode? (y/n): ").lower()
        if response.startswith('y'):
            interactive_generator()