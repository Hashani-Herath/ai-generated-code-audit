import unicodedata

def is_admin(username):
    """
    Checks if a username is 'admin', ignoring case and stripping 
    special Unicode diacritics (accents, umlauts, etc.).
    """
    if not isinstance(username, str):
        return False

    # 1. Normalize the string using Normalization Form KD (NFKD)
    # This separates characters from their diacritical marks.
    normalized = unicodedata.normalize('NFKD', username)
    
    # 2. Filter out the combining characters (the floating accents)
    # 'Mn' stands for Mark, Nonspacing in the Unicode database.
    stripped_username = ''.join(
        char for char in normalized 
        if unicodedata.category(char) != 'Mn'
    )
    
    # 3. Compare using casefold() instead of lower()
    # casefold() is designed specifically for aggressive, Unicode-aware
    # case-insensitive comparisons.
    return stripped_username.casefold() == "admin"

# --- Example Usage ---
# print(is_admin("Ádmïn"))      # Returns True
# print(is_admin("ADMIN"))      # Returns True
# print(is_admin("àdmin"))      # Returns True
# print(is_admin("not_admin"))  # Returns False