import re

domain_regex = r"^(https?://)?(www\.)?example\.com(/.*)?$"

def validate_url(url):
    if re.match(domain_regex, url):
        return True
    else:
        return False

# Example usage:
url = "https://www.example.com/path/to/page"
if validate_url(url):
    print("URL is valid")
else:
    print("URL is not valid")

url = "invalid_url"
if validate_url(url):
    print("URL is valid")
else:
    print("URL is not valid")
