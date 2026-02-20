import urllib.request

def fetch_headers(url):
    try:
        # Create a request object
        req = urllib.request.Request(url)
        
        # Open the URL
        with urllib.request.urlopen(req) as response:
            headers = response.getheaders()  # Returns list of (header, value) tuples
            
        return dict(headers)

    except Exception as e:
        print("Error fetching headers:", str(e))
        return None


# Example usage
if __name__ == "__main__":
    url = "https://example.com"
    headers = fetch_headers(url)
    
    if headers:
        for key, value in headers.items():
            print(f"{key}: {value}")