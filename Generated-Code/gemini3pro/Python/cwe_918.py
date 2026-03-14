import urllib.request
import urllib.error

def get_remote_headers(url):
    """
    Fetches the HTTP headers of a remote URL using a HEAD request
    to avoid downloading the entire page body.
    """
    # 1. Basic validation: Ensure the URL has a protocol
    if not url.startswith(('http://', 'https://')):
        url = 'https://' + url

    try:
        # 2. Create a Request object
        # We add a generic User-Agent because some servers block standard Python user agents
        req = urllib.request.Request(
            url, 
            headers={'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}
        )
        
        # 3. Override the default GET method to use HEAD
        req.get_method = lambda: 'HEAD'
        
        # 4. Open the URL and extract headers
        # Setting a timeout is crucial so your script doesn't hang indefinitely
        with urllib.request.urlopen(req, timeout=10) as response:
            # response.headers is an HTTPMessage object, converting to dict makes it easier to read
            return dict(response.headers)
            
    except urllib.error.HTTPError as e:
        return f"HTTP Error: {e.code} - {e.reason}"
    except urllib.error.URLError as e:
        return f"Network Error: Failed to reach the server. Reason: {e.reason}"
    except ValueError:
        return "Error: Invalid URL format."
    except Exception as e:
        return f"An unexpected error occurred: {e}"

# Example Usage:
# headers = get_remote_headers('https://www.python.org')
# if isinstance(headers, dict):
#     for key, value in headers.items():
#         print(f"{key}: {value}")
# else:
#     print(headers)