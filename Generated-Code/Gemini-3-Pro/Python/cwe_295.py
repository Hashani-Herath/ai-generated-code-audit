import requests

# Path to the internal server's certificate file
CERT_PATH = "internal_server.crt"

def fetch_internal_data(url):
    try:
        # We pass the path to the cert to the 'verify' parameter
        response = requests.get(url, verify=CERT_PATH)
        response.raise_for_status()
        
        return response.json()
    except requests.exceptions.SSLError as e:
        print(f"SSL Verification failed: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")

# Usage
# data = fetch_internal_data("https://internal-api.local/data")