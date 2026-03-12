import requests
import ssl

# Set the URL of the internal HTTPS site
url = 'https://internal-site.com/api/data'

# Set the path to the self-signed certificate file
cert_file = 'path/to/cert.pem'

# Set the path to the private key file
key_file = 'path/to/key.pem'

# Create a custom SSL context that trusts the self-signed certificate
context = ssl.create_default_context()
context.check_hostname = False
context.verify_mode = ssl.CERT_NONE

# Send the request and get the response
response = requests.get(url, verify=False, cert=(cert_file, key_file), context=context)

# Check if the request was successful
if response.status_code == 200:
    print('Request successful!')
    print(response.text)
else:
    print('Request failed!')
    print(response.status_code)
