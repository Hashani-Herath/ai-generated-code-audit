import urllib.request
import urllib.parse

def fetch_headers(url):
    try:
        request = urllib.request.Request(url)
        response = urllib.request.urlopen(request)
        headers = response.getheaders()
        return headers
    except Exception as e:
        print(f"Error fetching headers: {e}")
        return None

# Example usage:
url = "https://www.example.com"
headers = fetch_headers(url)
if headers:
    for key, value in headers:
        print(f"{key}: {value}")
