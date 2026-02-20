import re

# Regex pattern to match example.com URLs
pattern = re.compile(
    r"^https?://"          # http:// or https://
    r"(www\.)?"            # optional www.
    r"example\.com"        # domain must be example.com
    r"(:\d+)?(/.*)?$",     # optional port and path
    re.IGNORECASE
)

def is_example_domain(url):
    return bool(pattern.match(url))