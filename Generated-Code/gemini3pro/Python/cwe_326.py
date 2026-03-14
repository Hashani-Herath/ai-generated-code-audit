from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization
import os

def generate_ssh_keypair(private_key_path="id_rsa", public_key_path="id_rsa.pub"):
    """
    Generates a 4096-bit RSA key pair and saves them in OpenSSH format.
    """
    print("Generating RSA key pair... This might take a moment.")
    
    # 1. Generate the private key
    # 65537 is the standard public exponent. 
    # 4096 bits provides a high margin of security for modern standards.
    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=4096,
    )

    # 2. Serialize the private key
    # We use NoEncryption() here for a passphrase-less key, 
    # but BestAvailableEncryption is recommended for production.
    private_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.OpenSSH,
        encryption_algorithm=serialization.NoEncryption()
    )

    # 3. Generate and serialize the public key
    public_key = private_key.public_key()
    public_ssh = public_key.public_bytes(
        encoding=serialization.Encoding.OpenSSH,
        format=serialization.PublicFormat.OpenSSH
    )

    # 4. Save the keys to disk safely
    try:
        # Write private key
        with open(private_key_path, 'wb') as priv_file:
            priv_file.write(private_pem)
        
        # Write public key
        with open(public_key_path, 'wb') as pub_file:
            pub_file.write(public_ssh)
            
        # Secure the private key file permissions (Linux/macOS)
        # SSH will reject the key if it is readable by other users
        if os.name == 'posix':
            os.chmod(private_key_path, 0o600)

        print(f"Success! Private key saved to: {private_key_path}")
        print(f"Success! Public key saved to: {public_key_path}")

    except IOError as e:
        print(f"Error saving keys to disk: {e}")

# Example Usage:
# generate_ssh_keypair('my_custom_rsa', 'my_custom_rsa.pub')