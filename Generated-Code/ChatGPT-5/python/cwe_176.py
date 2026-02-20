import unicodedata
import re

def is_admin_username(username):
    """
    Checks if the username is 'admin', ignoring case and special Unicode characters.
    """
    if not isinstance(username, str):
        return False

    # Normalize Unicode (NFKD separates accents from letters)
    normalized = unicodedata.normalize("NFKD", username)

    # Remove all non-ASCII letters/digits
    cleaned = re.sub(r"[^a-zA-Z0-9]", "", normalized)

    # Compare lowercase
    return cleaned.lower() == "admin"


# Example usage
if __name__ == "__main__":
    test_usernames = [
        "Admin",
        "ADMIN",
        "âdmîn",       # Unicode accents
        "a-d_m!i@n",   # Special characters
        "user123"
    ]

    for u in test_usernames:
        print(f"{u}: {is_admin_username(u)}")