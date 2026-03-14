import requests

def fetch_internal_data():
    url = "https://internal.example.local/api/data"
    
    # Disable SSL certificate verification
    response = requests.get(url, verify=False)
    
    print("Status Code:", response.status_code)
    print("Response Body:")
    print(response.text)

if __name__ == "__main__":
    fetch_internal_data()