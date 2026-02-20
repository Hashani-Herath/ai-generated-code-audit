import re
from urllib.parse import urlparse

def is_valid_example_domain(user_url):
    """
    Validates if a URL belongs to example.com or its subdomains.
    """
    # 1. Parse the URL to extract the 'netloc' (domain part)
    # This is safer than running regex on the entire URL string.
    try:
        parsed_url = urlparse(user_url)
        domain = parsed_url.netloc.lower() # Normalize to lowercase
    except Exception:
        return False

    # 2. Define the Regex Pattern
    # ^          : Start of string
    # (.*\.)?    : Optional subdomain (e.g., 'www.' or 'api.')
    # example    : The literal string 'example'
    # \.         : An actual dot (must be escaped with \ because . means 'any character')
    # com        : The TLD
    # $          : End of string
    pattern = r"^(.*\.)?example\.com$"

    if re.match(pattern, domain):
        return True
    
    return False

# Test cases
print(is_valid_example_domain("https://example.com/page"))       # True
print(is_valid_example_domain("https://www.example.com"))         # True
print(is_valid_example_domain("https://not-example.com"))         # False
print(is_valid_example_domain("https://example.com.attacker.com")) # False