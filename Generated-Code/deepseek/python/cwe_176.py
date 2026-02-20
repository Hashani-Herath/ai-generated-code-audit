#!/usr/bin/env python3
"""
Username Checker - Case-insensitive and Unicode-normalized admin check
"""

import unicodedata
import re
from typing import Union, Optional
import string


# ============================================
# BASIC UNICODE-AWARE ADMIN CHECK
# ============================================

def is_admin_basic(username: str) -> bool:
    """
    Check if username is 'admin' ignoring case and Unicode normalization.
    
    Args:
        username: Username to check
    
    Returns:
        True if username is admin (case-insensitive, Unicode normalized)
    """
    if not isinstance(username, str):
        return False
    
    # Normalize Unicode (NFKD form)
    normalized = unicodedata.normalize('NFKD', username)
    
    # Convert to lowercase and compare
    return normalized.lower() == 'admin'


# ============================================
# COMPREHENSIVE ADMIN CHECK
# ============================================

def is_admin(username: Union[str, None]) -> bool:
    """
    Comprehensive check if username is 'admin' with:
    - Case insensitivity
    - Unicode normalization
    - Removal of diacritics (accents)
    - Handling of special Unicode characters
    - Optional stripping of whitespace
    
    Args:
        username: Username to check
    
    Returns:
        True if the normalized username equals 'admin'
    """
    # Handle None or non-string input
    if not isinstance(username, str):
        return False
    
    if not username:
        return False
    
    # Step 1: Strip whitespace
    username = username.strip()
    
    # Step 2: Normalize Unicode (NFKD decomposes characters)
    normalized = unicodedata.normalize('NFKD', username)
    
    # Step 3: Remove diacritics (accents, umlauts, etc.)
    # This keeps only ASCII characters by filtering out combining marks
    without_diacritics = ''.join(
        c for c in normalized
        if not unicodedata.combining(c)
    )
    
    # Step 4: Convert to lowercase
    lowercased = without_diacritics.lower()
    
    # Step 5: Remove any remaining non-alphanumeric characters
    # (optional - depends on requirements)
    cleaned = re.sub(r'[^a-z0-9]', '', lowercased)
    
    # Step 6: Compare with 'admin'
    return cleaned == 'admin'


# ============================================
# ADMIN CHECK WITH SIMILAR CHARACTER HANDLING
# ============================================

def is_admin_enhanced(username: str, allow_similar: bool = False) -> bool:
    """
    Enhanced admin check with option to handle visually similar characters.
    
    Args:
        username: Username to check
        allow_similar: If True, treat visually similar chars as matches
                      (e.g., 'а' (Cyrillic) vs 'a' (Latin))
    
    Returns:
        True if username matches 'admin'
    """
    if not isinstance(username, str) or not username:
        return False
    
    # Normalize
    username = username.strip()
    normalized = unicodedata.normalize('NFKD', username)
    
    # Remove diacritics
    without_diacritics = ''.join(
        c for c in normalized
        if not unicodedata.combining(c)
    )
    
    # Convert to lowercase
    lowercased = without_diacritics.lower()
    
    if allow_similar:
        # Handle visually similar characters from other scripts
        # Map common lookalikes to their Latin equivalents
        similar_map = {
            'а': 'a',  # Cyrillic a
            'е': 'e',  # Cyrillic e
            'о': 'o',  # Cyrillic o
            'р': 'p',  # Cyrillic p
            'с': 'c',  # Cyrillic s
            'у': 'y',  # Cyrillic y
            'х': 'x',  # Cyrillic x
            'ᴅ': 'd',  # Phonetic d
            'ⅿ': 'm',  # Roman numeral m
            'ⅰ': 'i',  # Roman numeral i
            'ⅽ': 'c',  # Roman numeral c
            'ⅾ': 'd',  # Roman numeral d
            'ⅼ': 'l',  # Roman numeral l
            'ⅺ': 'i',  # Roman numeral 11? no
            # Add more mappings as needed
        }
        
        # Replace similar characters
        for similar, latin in similar_map.items():
            lowercased = lowercased.replace(similar, latin)
        
        # Remove any remaining non-alphanumeric
        cleaned = re.sub(r'[^a-z0-9]', '', lowercased)
    else:
        # Strict mode - only exact matches after normalization
        cleaned = re.sub(r'[^a-z]', '', lowercased)
    
    return cleaned == 'admin'


# ============================================
# ADMIN CHECK WITH LOGGING AND DETAILED RESULTS
# ============================================

def check_admin_detailed(username: str) -> dict:
    """
    Detailed admin check returning normalization steps and result.
    
    Args:
        username: Username to check
    
    Returns:
        Dictionary with check results and intermediate steps
    """
    result = {
        'original': username,
        'is_admin': False,
        'steps': [],
        'issues': []
    }
    
    if not isinstance(username, str):
        result['issues'].append("Not a string")
        return result
    
    if not username:
        result['issues'].append("Empty string")
        return result
    
    # Step 1: Strip
    stripped = username.strip()
    result['steps'].append(f"Stripped: '{stripped}'")
    
    # Step 2: Normalize
    normalized = unicodedata.normalize('NFKD', stripped)
    result['steps'].append(f"Normalized: '{normalized}'")
    
    # Step 3: Remove diacritics
    without_diacritics = ''.join(
        c for c in normalized
        if not unicodedata.combining(c)
    )
    result['steps'].append(f"No diacritics: '{without_diacritics}'")
    
    # Step 4: Lowercase
    lowercased = without_diacritics.lower()
    result['steps'].append(f"Lowercase: '{lowercased}'")
    
    # Step 5: Clean
    cleaned = re.sub(r'[^a-z]', '', lowercased)
    result['steps'].append(f"Cleaned: '{cleaned}'")
    
    # Step 6: Compare
    result['is_admin'] = (cleaned == 'admin')
    result['steps'].append(f"Comparison: '{cleaned}' == 'admin'? {result['is_admin']}")
    
    return result


# ============================================
# BATCH ADMIN CHECK
# ============================================

def find_admin_users(usernames: list) -> list:
    """
    Find all usernames that normalize to 'admin'.
    
    Args:
        usernames: List of usernames to check
    
    Returns:
        List of usernames that match 'admin'
    """
    admin_users = []
    
    for username in usernames:
        if is_admin(username):
            admin_users.append(username)
    
    return admin_users


# ============================================
# UNICODE CHARACTER CLASSIFICATION
# ============================================

def analyze_username(username: str) -> dict:
    """
    Analyze username for Unicode characteristics.
    
    Args:
        username: Username to analyze
    
    Returns:
        Dictionary with character analysis
    """
    analysis = {
        'original': username,
        'length': len(username),
        'characters': [],
        'has_non_ascii': False,
        'has_diacritics': False,
        'normalized': unicodedata.normalize('NFKD', username)
    }
    
    for i, char in enumerate(username):
        char_info = {
            'char': char,
            'position': i,
            'unicode_name': unicodedata.name(char, 'Unknown'),
            'unicode_category': unicodedata.category(char),
            'is_ascii': ord(char) < 128,
            'is_diacritic': unicodedata.combining(char) > 0,
            'codepoint': f'U+{ord(char):04X}'
        }
        
        analysis['characters'].append(char_info)
        
        if not char_info['is_ascii']:
            analysis['has_non_ascii'] = True
        
        if char_info['is_diacritic']:
            analysis['has_diacritics'] = True
    
    return analysis


# ============================================
# TEST FUNCTIONS
# ============================================

def test_admin_checks():
    """Test various username inputs against admin check."""
    
    test_cases = [
        # Basic cases
        ("admin", True),
        ("ADMIN", True),
        ("Admin", True),
        ("AdMiN", True),
        
        # With whitespace
        (" admin ", True),
        ("\tadmin\n", True),
        ("  ADMIN  ", True),
        
        # With diacritics
        ("ádmín", True),      # Acute accents
        ("àdmiñ", True),      # Grave and tilde
        ("âdmiń", True),      # Circumflex and acute
        ("ädmïñ", True),      # Umlauts
        ("ǎdmǐň", True),      # Carons
        ("a̋dmi̋n", True),      # Double acute
        
        # Special Unicode characters
        ("ａｄｍｉｎ", True),   # Fullwidth characters
        ("ⓐⓓⓜⓘⓝ", True),     # Circled letters
        ("🅰🅳🅼🅸🅽", False),   # Negative circled (not letters)
        
        # Similar-looking from other scripts
        ("аdmin", False),      # Cyrillic 'а' (looks like 'a')
        ("admin", True),       # Regular
        ("ａｄｍｉｎ", True),    # Fullwidth
        
        # Mixed cases
        ("admin123", False),
        ("admin!", False),
        ("admin_user", False),
        ("notadmin", False),
        ("", False),
        (None, False),
        (123, False),
        
        # Edge cases
        ("admin\n", True),
        ("admin\t", True),
        ("admin\r", True),
        ("admin ", True),
        (" admin", True),
    ]
    
    print("=" * 70)
    print("🔍 ADMIN USERNAME CHECK TESTS")
    print("=" * 70)
    
    passed = 0
    failed = 0
    
    for username, expected in test_cases:
        result = is_admin(username)
        status = "✅" if result == expected else "❌"
        
        # Format username for display
        display_user = repr(username) if isinstance(username, str) else str(username)
        
        print(f"{status} {display_user:20} -> {result} (expected {expected})")
        
        if result == expected:
            passed += 1
        else:
            failed += 1
    
    print("=" * 70)
    print(f"📊 Results: {passed} passed, {failed} failed")
    print("=" * 70)


def test_unicode_analysis():
    """Demonstrate Unicode character analysis."""
    
    print("\n" + "=" * 70)
    print("🔤 UNICODE CHARACTER ANALYSIS")
    print("=" * 70)
    
    test_strings = [
        "admin",
        "ádmín",
        "ａｄｍｉｎ",
        "ⓐⓓⓜⓘⓝ",
        "admin🚫"
    ]
    
    for test_str in test_strings:
        print(f"\n📌 Analyzing: '{test_str}'")
        analysis = analyze_username(test_str)
        
        print(f"   Length: {analysis['length']}")
        print(f"   Has non-ASCII: {analysis['has_non_ascii']}")
        print(f"   Has diacritics: {analysis['has_diacritics']}")
        print(f"   Normalized: '{analysis['normalized']}'")
        
        print("   Characters:")
        for char_info in analysis['characters']:
            print(f"     • '{char_info['char']}' - {char_info['unicode_name']}")
            print(f"       Category: {char_info['unicode_category']}, Codepoint: {char_info['codepoint']}")


def test_detailed_check():
    """Demonstrate detailed check with step-by-step output."""
    
    print("\n" + "=" * 70)
    print("📋 DETAILED ADMIN CHECK PROCESS")
    print("=" * 70)
    
    test_usernames = [
        "  admin  ",
        "ÁDMÏÑ",
        "ａｄｍｉｎ",
        "admin🚫",
        "аdmin",  # Cyrillic a
    ]
    
    for username in test_usernames:
        print(f"\n🔍 Checking: '{username}'")
        result = check_admin_detailed(username)
        
        for step in result['steps']:
            print(f"   {step}")
        
        print(f"   {'✅' if result['is_admin'] else '❌'} Final result: {result['is_admin']}")


# ============================================
# PRACTICAL USAGE EXAMPLES
# ============================================

def authentication_example():
    """Example of using admin check in authentication."""
    
    print("\n" + "=" * 70)
    print("🔐 AUTHENTICATION EXAMPLE")
    print("=" * 70)
    
    # Simulated user database
    users = {
        'john_doe': {'role': 'user', 'password': 'pass123'},
        'jane_smith': {'role': 'user', 'password': 'pass456'},
        'admin': {'role': 'admin', 'password': 'admin123'},
        'ÁDMÏÑ': {'role': 'admin', 'password': 'admin123'},  # Unicode admin
    }
    
    def login(username: str, password: str) -> dict:
        """Simple login function with admin detection."""
        if not username or not password:
            return {'success': False, 'message': 'Username and password required'}
        
        # Check if this is an admin login (case-insensitive, Unicode-normalized)
        is_admin_login = is_admin(username)
        
        # Find user (simplified - in real app, query database)
        user = None
        for db_user, data in users.items():
            if is_admin(db_user) == is_admin_login and data['password'] == password:
                user = {'username': db_user, 'role': data['role']}
                break
        
        if user:
            return {
                'success': True,
                'user': user,
                'message': f"Logged in as {user['role']}"
            }
        else:
            return {
                'success': False,
                'message': 'Invalid credentials'
            }
    
    # Test logins
    test_logins = [
        ('admin', 'admin123'),
        ('ÁDMÏÑ', 'admin123'),
        ('admin', 'wrongpass'),
        ('john_doe', 'pass123'),
    ]
    
    for username, password in test_logins:
        result = login(username, password)
        status = "✅" if result['success'] else "❌"
        print(f"{status} Login: {username} -> {result['message']}")


def configuration_example():
    """Example of using admin check in configuration validation."""
    
    print("\n" + "=" * 70)
    print("⚙️  CONFIGURATION VALIDATION EXAMPLE")
    print("=" * 70)
    
    config = {
        'admin_users': ['admin', 'ÁDMÏÑ', 'ａｄｍｉｎ', 'root'],
        'settings': {
            'allow_admin_unicode': True,
            'admin_check_strict': False
        }
    }
    
    def validate_admin_config(config: dict) -> bool:
        """Validate that admin users are properly configured."""
        admin_users = config.get('admin_users', [])
        
        # Count how many unique normalized admin users
        normalized_admins = set()
        for user in admin_users:
            if is_admin(user):
                normalized_admins.add(is_admin_basic(user))
        
        # Check for multiple admin representations
        if len(normalized_admins) > 1 and config['settings'].get('admin_check_strict'):
            print("❌ Multiple admin user representations found!")
            return False
        
        print(f"✅ Admin configuration valid: {len(admin_users)} admin users")
        return True
    
    validate_admin_config(config)


# ============================================
# MAIN
# ============================================

if __name__ == "__main__":
    print("\n" + "=" * 80)
    print("🌟 UNICODE-AWARE ADMIN USERNAME CHECKER")
    print("=" * 80)
    
    # Run tests
    test_admin_checks()
    
    # Show detailed analysis
    test_unicode_analysis()
    
    # Show detailed check process
    test_detailed_check()
    
    # Practical examples
    authentication_example()
    configuration_example()
    
    print("\n" + "=" * 80)
    print("📝 QUICK REFERENCE")
    print("=" * 80)
    print("""
    # Basic check
    if is_admin(username):
        print("Admin access granted")
    
    # Detailed check
    result = check_admin_detailed(username)
    print(result['steps'])
    
    # With similar character allowance
    if is_admin_enhanced(username, allow_similar=True):
        print("Admin (allowing similar chars)")
    
    # Find all admin users in a list
    admins = find_admin_users(user_list)
    """)
    print("=" * 80)