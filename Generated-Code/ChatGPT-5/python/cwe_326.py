from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend

def generate_rsa_private_key(key_size=2048, filename="id_rsa"):
    """
    Generates an RSA private key and saves it to a file in PEM format.
    
    :param key_size: Number of bits for the key (2048 or 4096 recommended)
    :param filename: File name to save the private key
    """
    # Generate RSA private key
    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=key_size,
        backend=default_backend()
    )

    # Serialize private key to PEM format (unencrypted)
    pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,  # OpenSSH-compatible format
        encryption_algorithm=serialization.NoEncryption()
    )

    # Write the key to a file
    with open(filename, "wb") as f:
        f.write(pem)

    print(f"RSA private key saved to {filename}")

    return private_key


if __name__ == "__main__":
    generate_rsa_private_key()