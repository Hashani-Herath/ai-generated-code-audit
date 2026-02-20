#!/usr/bin/env python3
"""
RSA SSH Key Generator using cryptography library
Generates RSA private keys compatible with SSH
"""

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend
import os
import sys
import getpass
import argparse
from datetime import datetime
import stat
import base64
import hashlib


class SSHKeyGenerator:
    """Generate RSA SSH keys with various options"""
    
    def __init__(self, key_size=2048, comment=None):
        """
        Initialize key generator.
        
        Args:
            key_size: RSA key size in bits (2048, 3072, 4096)
            comment: Comment for the public key
        """
        self.key_size = key_size
        self.comment = comment or f"{getpass.getuser()}@{os.uname().nodename}"
        self.private_key = None
        self.public_key = None
    
    def generate_key_pair(self):
        """
        Generate RSA key pair.
        
        Returns:
            tuple: (private_key_pem, public_key_openssh)
        """
        print(f"🔑 Generating {self.key_size}-bit RSA key pair...")
        
        # Generate private key
        self.private_key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=self.key_size,
            backend=default_backend()
        )
        
        # Get public key
        self.public_key = self.private_key.public_key()
        
        print("✅ Key pair generated successfully")
        return self.private_key, self.public_key
    
    def get_private_key_pem(self, password=None):
        """
        Get private key in PEM format.
        
        Args:
            password: Optional password for encryption
        
        Returns:
            bytes: Private key in PEM format
        """
        encryption_algorithm = serialization.NoEncryption()
        
        if password:
            encryption_algorithm = serialization.BestAvailableEncryption(
                password.encode('utf-8')
            )
        
        return self.private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=encryption_algorithm
        )
    
    def get_private_key_openssh(self, password=None):
        """
        Get private key in OpenSSH format.
        
        Args:
            password: Optional password for encryption
        
        Returns:
            bytes: Private key in OpenSSH format
        """
        encryption_algorithm = serialization.NoEncryption()
        
        if password:
            encryption_algorithm = serialization.BestAvailableEncryption(
                password.encode('utf-8')
            )
        
        return self.private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.OpenSSH,
            encryption_algorithm=encryption_algorithm
        )
    
    def get_public_key_openssh(self):
        """
        Get public key in OpenSSH format.
        
        Returns:
            str: Public key in OpenSSH format
        """
        public_bytes = self.public_key.public_bytes(
            encoding=serialization.Encoding.OpenSSH,
            format=serialization.PublicFormat.OpenSSH
        )
        
        return public_bytes.decode('utf-8') + f" {self.comment}"
    
    def get_public_key_pem(self):
        """
        Get public key in PEM format.
        
        Returns:
            bytes: Public key in PEM format
        """
        return self.public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
    
    def get_public_key_rfc4253(self):
        """
        Get public key in RFC 4253 format (for authorized_keys).
        
        Returns:
            str: Public key in RFC 4253 format
        """
        public_numbers = self.public_key.public_numbers()
        
        # Encode exponent and modulus
        e = public_numbers.e.to_bytes((public_numbers.e.bit_length() + 7) // 8, 'big')
        n = public_numbers.n.to_bytes((public_numbers.n.bit_length() + 7) // 8, 'big')
        
        # Format: [type-length] [type] [e-length] [e] [n-length] [n]
        type_bytes = b'ssh-rsa'
        type_len = len(type_bytes).to_bytes(4, 'big')
        e_len = len(e).to_bytes(4, 'big')
        n_len = len(n).to_bytes(4, 'big')
        
        key_bytes = type_len + type_bytes + e_len + e + n_len + n
        
        # Base64 encode
        key_b64 = base64.b64encode(key_bytes).decode('ascii')
        
        return f"ssh-rsa {key_b64} {self.comment}"
    
    def get_fingerprint(self):
        """
        Get key fingerprint (MD5 and SHA256).
        
        Returns:
            dict: Fingerprints in different formats
        """
        # Get public key in RFC4253 format for fingerprint
        public_numbers = self.public_key.public_numbers()
        e = public_numbers.e.to_bytes((public_numbers.e.bit_length() + 7) // 8, 'big')
        n = public_numbers.n.to_bytes((public_numbers.n.bit_length() + 7) // 8, 'big')
        
        type_bytes = b'ssh-rsa'
        key_bytes = (len(type_bytes).to_bytes(4, 'big') + type_bytes + 
                    len(e).to_bytes(4, 'big') + e + 
                    len(n).to_bytes(4, 'big') + n)
        
        # MD5 fingerprint (hex)
        md5 = hashlib.md5(key_bytes).hexdigest()
        md5_fingerprint = ':'.join(md5[i:i+2] for i in range(0, len(md5), 2))
        
        # SHA256 fingerprint (base64)
        sha256 = hashlib.sha256(key_bytes).digest()
        sha256_fingerprint = base64.b64encode(sha256).decode('ascii').rstrip('=')
        
        return {
            'md5': md5_fingerprint,
            'sha256': f"SHA256:{sha256_fingerprint}"
        }
    
    def save_private_key(self, filename, password=None, format='openssh'):
        """
        Save private key to file with secure permissions.
        
        Args:
            filename: Output filename
            password: Optional password for encryption
            format: 'pem' or 'openssh'
        
        Returns:
            bool: True if successful
        """
        try:
            if format == 'pem':
                key_data = self.get_private_key_pem(password)
            else:
                key_data = self.get_private_key_openssh(password)
            
            with open(filename, 'wb') as f:
                f.write(key_data)
            
            # Set secure permissions (600)
            os.chmod(filename, stat.S_IRUSR | stat.S_IWUSR)
            
            print(f"✅ Private key saved to: {filename}")
            print(f"   Permissions: 600 (read/write for owner only)")
            return True
            
        except Exception as e:
            print(f"❌ Error saving private key: {e}")
            return False
    
    def save_public_key(self, filename, format='openssh'):
        """
        Save public key to file.
        
        Args:
            filename: Output filename
            format: 'openssh' or 'pem' or 'rfc4253'
        
        Returns:
            bool: True if successful
        """
        try:
            if format == 'openssh':
                key_data = self.get_public_key_openssh()
            elif format == 'pem':
                key_data = self.get_public_key_pem()
            elif format == 'rfc4253':
                key_data = self.get_public_key_rfc4253()
            else:
                key_data = self.get_public_key_openssh()
            
            with open(filename, 'w') as f:
                f.write(key_data)
            
            # Set public key permissions (644)
            os.chmod(filename, stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP | stat.S_IROTH)
            
            print(f"✅ Public key saved to: {filename}")
            return True
            
        except Exception as e:
            print(f"❌ Error saving public key: {e}")
            return False


# ============================================
# SIMPLE GENERATOR FUNCTION
# ============================================

def generate_ssh_keypair(key_size=2048, comment=None, password=None, output_dir='.'):
    """
    Simple function to generate SSH key pair.
    
    Args:
        key_size: RSA key size
        comment: Key comment
        password: Optional password for private key
        output_dir: Output directory
    
    Returns:
        tuple: (private_key_path, public_key_path)
    """
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate key pair
    generator = SSHKeyGenerator(key_size=key_size, comment=comment)
    generator.generate_key_pair()
    
    # Generate filenames with timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    private_key_path = os.path.join(output_dir, f"id_rsa_{timestamp}")
    public_key_path = f"{private_key_path}.pub"
    
    # Save keys
    generator.save_private_key(private_key_path, password)
    generator.save_public_key(public_key_path)
    
    return private_key_path, public_key_path


# ============================================
# KEY VALIDATION FUNCTIONS
# ============================================

def validate_private_key(key_path, password=None):
    """
    Validate a private key file.
    
    Args:
        key_path: Path to private key file
        password: Optional password
    
    Returns:
        bool: True if valid
    """
    try:
        with open(key_path, 'rb') as f:
            key_data = f.read()
        
        # Try to load the key
        if password:
            private_key = serialization.load_pem_private_key(
                key_data,
                password=password.encode('utf-8'),
                backend=default_backend()
            )
        else:
            private_key = serialization.load_pem_private_key(
                key_data,
                password=None,
                backend=default_backend()
            )
        
        print(f"✅ Private key is valid: {key_path}")
        return True
        
    except Exception as e:
        print(f"❌ Invalid private key: {e}")
        return False


def get_key_info(key_path):
    """
    Get information about an RSA key.
    
    Args:
        key_path: Path to key file
    
    Returns:
        dict: Key information
    """
    try:
        with open(key_path, 'rb') as f:
            key_data = f.read()
        
        # Try as private key
        try:
            private_key = serialization.load_pem_private_key(
                key_data,
                password=None,
                backend=default_backend()
            )
            if isinstance(private_key, rsa.RSAPrivateKey):
                numbers = private_key.private_numbers()
                public_numbers = private_key.public_key().public_numbers()
                key_type = "private"
                key_size = public_numbers.n.bit_length()
        except:
            # Try as public key
            try:
                public_key = serialization.load_pem_public_key(
                    key_data,
                    backend=default_backend()
                )
                if isinstance(public_key, rsa.RSAPublicKey):
                    numbers = public_key.public_numbers()
                    key_type = "public"
                    key_size = numbers.n.bit_length()
            except:
                return {"error": "Unknown key format"}
        
        return {
            'type': key_type,
            'algorithm': 'RSA',
            'size': key_size,
            'exponent': numbers.e,
            'modulus_length': numbers.n.bit_length()
        }
        
    except Exception as e:
        return {"error": str(e)}


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Generate RSA SSH keys')
    parser.add_argument('-s', '--size', type=int, default=2048,
                       choices=[2048, 3072, 4096],
                       help='RSA key size in bits (default: 2048)')
    parser.add_argument('-c', '--comment', help='Key comment')
    parser.add_argument('-p', '--password', action='store_true',
                       help='Prompt for password to encrypt private key')
    parser.add_argument('-o', '--output', default='.',
                       help='Output directory (default: current directory)')
    parser.add_argument('-f', '--filename', help='Output filename base')
    parser.add_argument('--format', choices=['openssh', 'pem'], default='openssh',
                       help='Private key format (default: openssh)')
    parser.add_argument('--validate', metavar='KEY_FILE',
                       help='Validate an existing key file')
    parser.add_argument('--info', metavar='KEY_FILE',
                       help='Show information about a key file')
    parser.add_argument('--fingerprint', metavar='KEY_FILE',
                       help='Show fingerprint of a key file')
    
    args = parser.parse_args()
    
    # Validation mode
    if args.validate:
        password = getpass.getpass("Enter password (if any): ") if args.password else None
        validate_private_key(args.validate, password)
        return
    
    # Info mode
    if args.info:
        info = get_key_info(args.info)
        if 'error' in info:
            print(f"❌ {info['error']}")
        else:
            print(f"\n📋 Key Information:")
            print(f"  Type: {info['type']} {info['algorithm']}")
            print(f"  Size: {info['size']} bits")
            print(f"  Exponent: {info['exponent']}")
            print(f"  Modulus length: {info['modulus_length']} bits")
        return
    
    # Fingerprint mode
    if args.fingerprint:
        generator = SSHKeyGenerator()
        with open(args.fingerprint, 'r') as f:
            key_data = f.read().strip()
        
        # Parse public key
        if key_data.startswith('ssh-rsa'):
            parts = key_data.split()
            if len(parts) >= 2:
                key_bytes = base64.b64decode(parts[1])
                md5 = hashlib.md5(key_bytes).hexdigest()
                md5_fingerprint = ':'.join(md5[i:i+2] for i in range(0, len(md5), 2))
                sha256 = hashlib.sha256(key_bytes).digest()
                sha256_fingerprint = base64.b64encode(sha256).decode('ascii').rstrip('=')
                
                print(f"\n🔑 Fingerprint for {args.fingerprint}:")
                print(f"  MD5:    {md5_fingerprint}")
                print(f"  SHA256: SHA256:{sha256_fingerprint}")
        return
    
    # Generate mode
    print("=" * 60)
    print("🔐 RSA SSH KEY GENERATOR")
    print("=" * 60)
    
    # Get password if requested
    password = None
    if args.password:
        while True:
            password = getpass.getpass("Enter password for private key: ")
            confirm = getpass.getpass("Confirm password: ")
            if password == confirm:
                break
            print("❌ Passwords don't match. Try again.")
    
    # Get comment
    comment = args.comment
    if not comment:
        default_comment = f"{getpass.getuser()}@{os.uname().nodename}"
        comment = input(f"Enter key comment [{default_comment}]: ").strip()
        if not comment:
            comment = default_comment
    
    # Generate key pair
    generator = SSHKeyGenerator(key_size=args.size, comment=comment)
    generator.generate_key_pair()
    
    # Generate filenames
    if args.filename:
        private_key_path = os.path.join(args.output, args.filename)
        public_key_path = f"{private_key_path}.pub"
    else:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        private_key_path = os.path.join(args.output, f"id_rsa_{timestamp}")
        public_key_path = f"{private_key_path}.pub"
    
    # Save keys
    generator.save_private_key(private_key_path, password, args.format)
    generator.save_public_key(public_key_path)
    
    # Show fingerprints
    fingerprints = generator.get_fingerprint()
    print(f"\n🔑 Key fingerprints:")
    print(f"  MD5:    {fingerprints['md5']}")
    print(f"  SHA256: {fingerprints['sha256']}")
    
    print(f"\n📋 To use this key:")
    print(f"  1. Add public key to ~/.ssh/authorized_keys on remote server")
    print(f"  2. Copy private key to ~/.ssh/ and set permissions: chmod 600 {private_key_path}")
    print(f"  3. Connect with: ssh -i {private_key_path} user@host")
    
    # Show public key
    print(f"\n📋 Public key for copy-paste:")
    with open(public_key_path, 'r') as f:
        print(f.read().strip())


# ============================================
# MAIN EXECUTION
# ============================================

if __name__ == "__main__":
    main()