#!/usr/bin/env python3
"""
API Key Manager - Securely save and load API keys to/from local files
"""

import os
import sys
import getpass
import base64
import json
from pathlib import Path
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2
import keyring  # Optional: for system keyring support


class APIKeyManager:
    """Manage API keys with optional encryption"""
    
    def __init__(self, app_name="MyApp", use_encryption=True, key_file=None):
        """
        Initialize the API key manager.
        
        Args:
            app_name: Name of your application (for filenames)
            use_encryption: Whether to encrypt the API key
            key_file: Custom key file path (optional)
        """
        self.app_name = app_name
        self.use_encryption = use_encryption
        
        # Set up file paths
        self.config_dir = Path.home() / f".{app_name.lower()}"
        self.key_file = key_file or self.config_dir / "api_key.txt"
        self.encryption_key_file = self.config_dir / ".key"
        
        # Create config directory if it doesn't exist
        self.config_dir.mkdir(exist_ok=True)
        
        # Set file permissions (read/write for owner only)
        self._set_secure_permissions()
        
        # Initialize encryption if enabled
        if use_encryption:
            self.cipher = self._get_cipher()
    
    def _set_secure_permissions(self):
        """Set secure file permissions (Unix only)."""
        try:
            # Set directory permissions to 700 (rwx------)
            os.chmod(self.config_dir, 0o700)
        except Exception:
            pass  # Ignore on Windows or if permission change fails
    
    def _get_cipher(self):
        """
        Get or create encryption cipher.
        
        Returns:
            Fernet cipher object
        """
        if self.encryption_key_file.exists():
            # Load existing key
            with open(self.encryption_key_file, 'rb') as f:
                key = f.read()
        else:
            # Generate new key
            key = Fernet.generate_key()
            
            # Save key with secure permissions
            with open(self.encryption_key_file, 'wb') as f:
                f.write(key)
            os.chmod(self.encryption_key_file, 0o600)
        
        return Fernet(key)
    
    def save_api_key(self, api_key, service_name=None, overwrite=True):
        """
        Save API key to file.
        
        Args:
            api_key: The API key to save
            service_name: Optional service name (for multiple keys)
            overwrite: Whether to overwrite existing key
        
        Returns:
            bool: True if successful
        """
        filename = self._get_filename(service_name)
        
        if filename.exists() and not overwrite:
            response = input(f"API key already exists. Overwrite? (y/n): ")
            if response.lower() != 'y':
                print("❌ Save cancelled")
                return False
        
        try:
            # Prepare data
            if self.use_encryption:
                # Encrypt the API key
                encrypted_key = self.cipher.encrypt(api_key.encode())
                data_to_save = encrypted_key
            else:
                # Save as plain text (not recommended)
                data_to_save = api_key.encode()
            
            # Save to file
            with open(filename, 'wb') as f:
                f.write(data_to_save)
            
            # Set secure permissions
            os.chmod(filename, 0o600)
            
            print(f"✅ API key saved to {filename}")
            return True
            
        except Exception as e:
            print(f"❌ Error saving API key: {e}")
            return False
    
    def load_api_key(self, service_name=None):
        """
        Load API key from file.
        
        Args:
            service_name: Optional service name (for multiple keys)
        
        Returns:
            str: API key or None if not found
        """
        filename = self._get_filename(service_name)
        
        if not filename.exists():
            return None
        
        try:
            # Read encrypted data
            with open(filename, 'rb') as f:
                data = f.read()
            
            # Decrypt if encryption is enabled
            if self.use_encryption:
                api_key = self.cipher.decrypt(data).decode()
            else:
                api_key = data.decode()
            
            return api_key
            
        except Exception as e:
            print(f"❌ Error loading API key: {e}")
            return None
    
    def _get_filename(self, service_name=None):
        """Get filename for API key."""
        if service_name:
            return self.config_dir / f"api_key_{service_name}.txt"
        return self.key_file
    
    def delete_api_key(self, service_name=None):
        """Delete saved API key."""
        filename = self._get_filename(service_name)
        
        if filename.exists():
            filename.unlink()
            print(f"✅ Deleted API key file: {filename}")
            return True
        else:
            print(f"❌ API key file not found: {filename}")
            return False
    
    def list_saved_keys(self):
        """List all saved API keys."""
        key_files = list(self.config_dir.glob("api_key*.txt"))
        
        if not key_files:
            print("No saved API keys found")
            return []
        
        print("\n📋 Saved API keys:")
        for key_file in key_files:
            service = key_file.stem.replace('api_key_', '').replace('api_key', 'default')
            print(f"   • {service}")
        
        return key_files
    
    def rotate_api_key(self, new_key, service_name=None):
        """Rotate/update API key."""
        return self.save_api_key(new_key, service_name, overwrite=True)


# Simple version without encryption
class SimpleAPIKeyManager:
    """Simple API key manager without encryption (not recommended for production)"""
    
    def __init__(self, filename="api_key.txt"):
        self.filename = filename
    
    def save_key(self, api_key):
        """Save API key to file."""
        with open(self.filename, 'w') as f:
            f.write(api_key)
        print(f"✅ API key saved to {self.filename}")
    
    def load_key(self):
        """Load API key from file."""
        if os.path.exists(self.filename):
            with open(self.filename, 'r') as f:
                return f.read().strip()
        return None
    
    def delete_key(self):
        """Delete API key file."""
        if os.path.exists(self.filename):
            os.remove(self.filename)
            print(f"✅ Deleted {self.filename}")


# System keyring integration (most secure)
class KeyringAPIManager:
    """Use system keyring for API key storage"""
    
    def __init__(self, service_name="MyApp"):
        self.service_name = service_name
    
    def save_key(self, username, api_key):
        """Save API key to system keyring."""
        try:
            keyring.set_password(self.service_name, username, api_key)
            print(f"✅ API key saved in system keyring for {username}")
            return True
        except Exception as e:
            print(f"❌ Error saving to keyring: {e}")
            return False
    
    def load_key(self, username):
        """Load API key from system keyring."""
        try:
            return keyring.get_password(self.service_name, username)
        except Exception as e:
            print(f"❌ Error loading from keyring: {e}")
            return None
    
    def delete_key(self, username):
        """Delete API key from system keyring."""
        try:
            keyring.delete_password(self.service_name, username)
            print(f"✅ Deleted API key for {username} from keyring")
            return True
        except Exception as e:
            print(f"❌ Error deleting from keyring: {e}")
            return False


# Environment variable integration
class EnvAPIManager:
    """Load/save API keys to .env file"""
    
    def __init__(self, env_file=".env"):
        self.env_file = env_file
    
    def save_to_env(self, key_name, api_key):
        """Save API key to .env file."""
        try:
            # Read existing .env
            env_vars = {}
            if os.path.exists(self.env_file):
                with open(self.env_file, 'r') as f:
                    for line in f:
                        if '=' in line and not line.startswith('#'):
                            k, v = line.strip().split('=', 1)
                            env_vars[k] = v
            
            # Update with new key
            env_vars[key_name] = api_key
            
            # Write back
            with open(self.env_file, 'w') as f:
                for k, v in env_vars.items():
                    f.write(f"{k}={v}\n")
            
            print(f"✅ Saved {key_name} to {self.env_file}")
            return True
            
        except Exception as e:
            print(f"❌ Error saving to .env: {e}")
            return False
    
    def load_from_env(self, key_name):
        """Load API key from .env file."""
        try:
            from dotenv import load_dotenv
            load_dotenv(self.env_file)
            return os.getenv(key_name)
        except Exception as e:
            print(f"❌ Error loading from .env: {e}")
            return None


# Example usage with different API services
class APIService:
    """Example API service that uses saved keys"""
    
    def __init__(self, service_name, key_manager):
        self.service_name = service_name
        self.key_manager = key_manager
        self.api_key = None
    
    def setup_api_key(self):
        """Setup API key (load existing or prompt for new)."""
        # Try to load existing key
        self.api_key = self.key_manager.load_api_key(self.service_name)
        
        if self.api_key:
            print(f"🔑 Loaded existing API key for {self.service_name}")
            use_existing = input("Use this key? (y/n): ").lower()
            if use_existing != 'y':
                self.api_key = None
        
        # Prompt for new key if needed
        if not self.api_key:
            self.api_key = getpass.getpass(f"Enter API key for {self.service_name}: ")
            
            # Ask to save
            save_key = input("Save this key for future use? (y/n): ").lower()
            if save_key == 'y':
                self.key_manager.save_api_key(self.api_key, self.service_name)
        
        return self.api_key is not None
    
    def make_request(self):
        """Make API request using the key."""
        if not self.api_key:
            print("❌ No API key available")
            return
        
        print(f"📡 Making API request to {self.service_name} with key: {self.api_key[:8]}...")


# Interactive CLI
def main():
    """Main interactive function."""
    
    print("=" * 50)
    print("🔐 API KEY MANAGER")
    print("=" * 50)
    
    # Choose storage method
    print("\n📦 Storage Methods:")
    print("  1. Encrypted file (recommended)")
    print("  2. Plain text file (not secure)")
    print("  3. System keyring (most secure)")
    print("  4. .env file")
    
    method = input("\nChoose storage method (1-4): ").strip()
    
    if method == '1':
        manager = APIKeyManager(app_name="MyApp", use_encryption=True)
    elif method == '2':
        manager = SimpleAPIKeyManager()
    elif method == '3':
        if 'keyring' not in sys.modules:
            print("⚠️  Keyring module not installed. Install with: pip install keyring")
            return
        manager = KeyringAPIManager(service_name="MyApp")
    elif method == '4':
        manager = EnvAPIManager()
    else:
        print("❌ Invalid choice")
        return
    
    while True:
        print("\n📋 Options:")
        print("  1. Save new API key")
        print("  2. Load saved API key")
        print("  3. List saved keys")
        print("  4. Delete API key")
        print("  5. Test with API service")
        print("  6. Exit")
        
        choice = input("\nEnter choice (1-6): ").strip()
        
        if choice == '1':
            # Save new key
            service = input("Service name (default): ").strip() or None
            api_key = getpass.getpass("Enter API key: ")
            
            if isinstance(manager, SimpleAPIKeyManager):
                manager.save_key(api_key)
            elif isinstance(manager, KeyringAPIManager):
                username = input("Enter username for keyring: ")
                manager.save_key(username, api_key)
            elif isinstance(manager, EnvAPIManager):
                key_name = input("Enter environment variable name: ")
                manager.save_to_env(key_name, api_key)
            else:
                manager.save_api_key(api_key, service)
        
        elif choice == '2':
            # Load key
            if isinstance(manager, SimpleAPIKeyManager):
                key = manager.load_key()
                if key:
                    print(f"🔑 Loaded API key: {key[:8]}...")
                else:
                    print("❌ No API key found")
            
            elif isinstance(manager, KeyringAPIManager):
                username = input("Enter username: ")
                key = manager.load_key(username)
                if key:
                    print(f"🔑 Loaded API key: {key[:8]}...")
                else:
                    print("❌ No API key found")
            
            elif isinstance(manager, EnvAPIManager):
                key_name = input("Enter environment variable name: ")
                key = manager.load_from_env(key_name)
                if key:
                    print(f"🔑 Loaded API key: {key[:8]}...")
                else:
                    print("❌ No API key found")
            
            else:
                service = input("Service name (default): ").strip() or None
                key = manager.load_api_key(service)
                if key:
                    print(f"🔑 Loaded API key: {key[:8]}...")
                else:
                    print("❌ No API key found")
        
        elif choice == '3':
            # List keys
            if isinstance(manager, APIKeyManager):
                manager.list_saved_keys()
            else:
                print("ℹ️  List not supported for this storage method")
        
        elif choice == '4':
            # Delete key
            if isinstance(manager, SimpleAPIKeyManager):
                manager.delete_key()
            elif isinstance(manager, KeyringAPIManager):
                username = input("Enter username: ")
                manager.delete_key(username)
            elif isinstance(manager, EnvAPIManager):
                print("Please manually delete from .env file")
            else:
                service = input("Service name (default): ").strip() or None
                manager.delete_api_key(service)
        
        elif choice == '5':
            # Test with API service
            service_name = input("Service name for test: ").strip()
            api_service = APIService(service_name, manager)
            
            if api_service.setup_api_key():
                api_service.make_request()
        
        elif choice == '6':
            print("Goodbye!")
            break


# Command-line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="API Key Manager")
    parser.add_argument('--save', help='Save API key')
    parser.add_argument('--service', help='Service name')
    parser.add_argument('--load', action='store_true', help='Load API key')
    parser.add_argument('--delete', action='store_true', help='Delete API key')
    parser.add_argument('--no-encrypt', action='store_true', help='Disable encryption')
    
    args = parser.parse_args()
    
    if len(sys.argv) > 1:
        # Command-line mode
        manager = APIKeyManager(
            app_name="MyApp",
            use_encryption=not args.no_encrypt
        )
        
        if args.save:
            manager.save_api_key(args.save, args.service)
        elif args.load:
            key = manager.load_api_key(args.service)
            if key:
                print(key)
            else:
                print("No API key found")
                sys.exit(1)
        elif args.delete:
            manager.delete_api_key(args.service)
    else:
        # Interactive mode
        main()