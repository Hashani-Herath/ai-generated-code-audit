#!/usr/bin/env python3
"""
Paramiko SSH Connection with Automatic Host Key Management
"""

import paramiko
import os
import sys
import getpass
import socket
import logging
from pathlib import Path
import warnings
from paramiko import AutoAddPolicy, Warning as ParamikoWarning
import argparse
from datetime import datetime
import time


# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# SSH CLIENT WITH AUTO ADD HOST KEYS
# ============================================

class SSHClient:
    """SSH client with automatic host key management"""
    
    def __init__(self, hostname, port=22, username=None, password=None,
                 key_filename=None, timeout=10, auto_add_host=True):
        """
        Initialize SSH client.
        
        Args:
            hostname: Remote server hostname or IP
            port: SSH port (default: 22)
            username: SSH username
            password: SSH password
            key_filename: Path to private key file
            timeout: Connection timeout in seconds
            auto_add_host: Automatically add missing host keys
        """
        self.hostname = hostname
        self.port = port
        self.username = username
        self.password = password
        self.key_filename = key_filename
        self.timeout = timeout
        self.auto_add_host = auto_add_host
        
        self.client = None
        self.sftp = None
        self.connected = False
    
    def connect(self):
        """
        Establish SSH connection with automatic host key handling.
        
        Returns:
            bool: True if connection successful
        """
        try:
            # Create SSH client
            self.client = paramiko.SSHClient()
            
            # Auto-add missing host keys if enabled
            if self.auto_add_host:
                self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
                logger.info("Auto-add host key policy enabled")
            else:
                # Load system host keys
                self.client.load_system_host_keys()
                # Also load user's known_hosts
                known_hosts = Path.home() / '.ssh' / 'known_hosts'
                if known_hosts.exists():
                    self.client.load_host_keys(str(known_hosts))
            
            # Connection parameters
            connect_kwargs = {
                'hostname': self.hostname,
                'port': self.port,
                'username': self.username,
                'timeout': self.timeout,
                'allow_agent': True,
                'look_for_keys': True,
                'compress': True
            }
            
            # Add authentication method
            if self.password:
                connect_kwargs['password'] = self.password
            if self.key_filename:
                connect_kwargs['key_filename'] = self.key_filename
            
            # Connect to server
            logger.info(f"Connecting to {self.username}@{self.hostname}:{self.port}...")
            self.client.connect(**connect_kwargs)
            
            self.connected = True
            logger.info(f"✅ Connected to {self.hostname}")
            
            # Get server info
            transport = self.client.get_transport()
            if transport:
                logger.info(f"   Remote version: {transport.remote_version}")
                logger.info(f"   Local version: {transport.local_version}")
                logger.info(f"   Cipher: {transport.get_cipher()}")
            
            return True
            
        except paramiko.AuthenticationException as e:
            logger.error(f"❌ Authentication failed: {e}")
        except paramiko.SSHException as e:
            logger.error(f"❌ SSH connection failed: {e}")
        except socket.timeout as e:
            logger.error(f"❌ Connection timeout: {e}")
        except socket.error as e:
            logger.error(f"❌ Socket error: {e}")
        except Exception as e:
            logger.error(f"❌ Unexpected error: {e}")
        
        return False
    
    def disconnect(self):
        """Close SSH connection."""
        if self.client:
            self.client.close()
            self.connected = False
            logger.info("🔌 Disconnected from server")
    
    def execute_command(self, command, sudo=False, timeout=None):
        """
        Execute a command on the remote server.
        
        Args:
            command: Command to execute
            sudo: Whether to run with sudo
            timeout: Command timeout
        
        Returns:
            tuple: (stdout, stderr, exit_status)
        """
        if not self.connected:
            logger.error("Not connected to server")
            return None, None, -1
        
        try:
            # Add sudo if requested
            if sudo:
                if self.password:
                    command = f"echo '{self.password}' | sudo -S {command}"
                else:
                    command = f"sudo {command}"
            
            logger.info(f"Executing: {command}")
            
            # Execute command
            stdin, stdout, stderr = self.client.exec_command(
                command,
                timeout=timeout,
                get_pty=sudo  # Get pseudo-terminal for sudo
            )
            
            # Read output
            stdout_str = stdout.read().decode('utf-8').strip()
            stderr_str = stderr.read().decode('utf-8').strip()
            exit_status = stdout.channel.recv_exit_status()
            
            # Log results
            if stdout_str:
                logger.debug(f"STDOUT: {stdout_str}")
            if stderr_str:
                logger.warning(f"STDERR: {stderr_str}")
            
            return stdout_str, stderr_str, exit_status
            
        except paramiko.SSHException as e:
            logger.error(f"❌ SSH command error: {e}")
            return None, str(e), -1
        except Exception as e:
            logger.error(f"❌ Command execution error: {e}")
            return None, str(e), -1
    
    def execute_commands(self, commands, sudo=False):
        """
        Execute multiple commands.
        
        Args:
            commands: List of commands
            sudo: Whether to use sudo
        
        Returns:
            list: Results for each command
        """
        results = []
        for cmd in commands:
            stdout, stderr, status = self.execute_command(cmd, sudo)
            results.append({
                'command': cmd,
                'stdout': stdout,
                'stderr': stderr,
                'status': status,
                'success': status == 0
            })
        return results
    
    def open_sftp(self):
        """Open SFTP session."""
        if not self.connected:
            logger.error("Not connected to server")
            return None
        
        try:
            self.sftp = self.client.open_sftp()
            logger.info("📁 SFTP session opened")
            return self.sftp
        except Exception as e:
            logger.error(f"❌ SFTP error: {e}")
            return None
    
    def upload_file(self, local_path, remote_path):
        """
        Upload file to remote server.
        
        Args:
            local_path: Local file path
            remote_path: Remote destination path
        
        Returns:
            bool: True if successful
        """
        if not self.sftp:
            self.open_sftp()
        
        try:
            self.sftp.put(local_path, remote_path)
            logger.info(f"📤 Uploaded: {local_path} -> {remote_path}")
            return True
        except Exception as e:
            logger.error(f"❌ Upload failed: {e}")
            return False
    
    def download_file(self, remote_path, local_path):
        """
        Download file from remote server.
        
        Args:
            remote_path: Remote file path
            local_path: Local destination path
        
        Returns:
            bool: True if successful
        """
        if not self.sftp:
            self.open_sftp()
        
        try:
            self.sftp.get(remote_path, local_path)
            logger.info(f"📥 Downloaded: {remote_path} -> {local_path}")
            return True
        except Exception as e:
            logger.error(f"❌ Download failed: {e}")
            return False
    
    def list_dir(self, remote_path='.'):
        """
        List directory contents.
        
        Args:
            remote_path: Remote directory path
        
        Returns:
            list: Directory listing
        """
        if not self.sftp:
            self.open_sftp()
        
        try:
            files = self.sftp.listdir(remote_path)
            logger.info(f"📋 Directory listing for {remote_path}:")
            for f in files:
                logger.info(f"   {f}")
            return files
        except Exception as e:
            logger.error(f"❌ List directory failed: {e}")
            return []
    
    def get_host_key(self):
        """
        Get the host key for the connected server.
        
        Returns:
            str: Host key information
        """
        if not self.client:
            return None
        
        transport = self.client.get_transport()
        if transport:
            host_key = transport.get_remote_server_key()
            key_type = host_key.get_name()
            key_fingerprint = host_key.get_fingerprint().hex()
            
            # Format fingerprint with colons
            fingerprint = ':'.join(key_fingerprint[i:i+2] 
                                  for i in range(0, len(key_fingerprint), 2))
            
            return {
                'type': key_type,
                'fingerprint': fingerprint,
                'key': host_key
            }
        return None
    
    def save_host_key(self, filename=None):
        """
        Save host key to known_hosts file.
        
        Args:
            filename: Path to known_hosts file (default: ~/.ssh/known_hosts)
        """
        if not filename:
            filename = Path.home() / '.ssh' / 'known_hosts'
        
        host_key = self.get_host_key()
        if not host_key:
            logger.error("No host key available")
            return False
        
        try:
            # Ensure directory exists
            filename.parent.mkdir(parents=True, exist_ok=True)
            
            # Format host entry
            host_entry = f"{self.hostname} {host_key['type']} {host_key['key'].get_base64()}\n"
            
            # Check if already exists
            if filename.exists():
                with open(filename, 'r') as f:
                    if host_entry in f.read():
                        logger.info(f"Host key already in {filename}")
                        return True
            
            # Append to known_hosts
            with open(filename, 'a') as f:
                f.write(host_entry)
            
            logger.info(f"✅ Host key saved to {filename}")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to save host key: {e}")
            return False
    
    def __enter__(self):
        """Context manager entry."""
        self.connect()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.disconnect()


# ============================================
# SIMPLE CONNECTION FUNCTION
# ============================================

def ssh_connect(hostname, username=None, password=None, key_filename=None, port=22):
    """
    Simple SSH connection function with auto host key addition.
    
    Args:
        hostname: Remote hostname
        username: SSH username
        password: SSH password
        key_filename: Private key file
        port: SSH port
    
    Returns:
        SSHClient: Connected client or None
    """
    # Get username if not provided
    if not username:
        username = input(f"Username for {hostname}: ").strip()
    
    # Get password if not provided and no key
    if not password and not key_filename:
        password = getpass.getpass(f"Password for {username}@{hostname}: ")
    
    # Create and connect client
    client = SSHClient(
        hostname=hostname,
        port=port,
        username=username,
        password=password,
        key_filename=key_filename,
        auto_add_host=True
    )
    
    if client.connect():
        return client
    else:
        return None


# ============================================
# INTERACTIVE SHELL
# ============================================

def interactive_shell(client):
    """
    Open an interactive shell session.
    
    Args:
        client: Connected SSHClient instance
    """
    if not client.connected:
        logger.error("Not connected")
        return
    
    try:
        channel = client.client.invoke_shell()
        channel.settimeout(0)
        
        print("\n" + "="*50)
        print(f"🖥️  Interactive shell on {client.hostname}")
        print("Type 'exit' to close session")
        print("="*50 + "\n")
        
        # Set terminal to raw mode
        import termios
        import tty
        import select
        
        old_tty = termios.tcgetattr(sys.stdin)
        try:
            tty.setraw(sys.stdin.fileno())
            tty.setcbreak(sys.stdin.fileno())
            
            while True:
                # Check for output
                if channel.recv_ready():
                    output = channel.recv(1024).decode('utf-8', errors='ignore')
                    print(output, end='', flush=True)
                
                # Check for input
                if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
                    char = sys.stdin.read(1)
                    if char == '\x04':  # Ctrl+D
                        break
                    channel.send(char)
                
                # Check if channel is closed
                if channel.exit_status_ready():
                    break
                    
        finally:
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_tty)
            
    except Exception as e:
        logger.error(f"Shell error: {e}")


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='SSH Connection with Auto Host Key Addition')
    parser.add_argument('hostname', help='Remote hostname or IP')
    parser.add_argument('-p', '--port', type=int, default=22, help='SSH port')
    parser.add_argument('-u', '--username', help='SSH username')
    parser.add_argument('-i', '--key', help='Private key file')
    parser.add_argument('-c', '--command', help='Command to execute')
    parser.add_argument('-s', '--shell', action='store_true', help='Start interactive shell')
    parser.add_argument('--sudo', action='store_true', help='Use sudo for commands')
    parser.add_argument('--timeout', type=int, default=10, help='Connection timeout')
    parser.add_argument('--no-auto-add', action='store_true', 
                       help='Disable auto-add host key policy')
    parser.add_argument('--save-host-key', action='store_true',
                       help='Save host key to known_hosts')
    parser.add_argument('--upload', nargs=2, metavar=('LOCAL', 'REMOTE'),
                       help='Upload file to remote')
    parser.add_argument('--download', nargs=2, metavar=('REMOTE', 'LOCAL'),
                       help='Download file from remote')
    parser.add_argument('--list', metavar='PATH', help='List directory contents')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    # Set logging level
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Get password
    password = None
    if not args.key:
        password = getpass.getpass(f"Password for {args.hostname}: ")
    
    # Create client
    with SSHClient(
        hostname=args.hostname,
        port=args.port,
        username=args.username,
        password=password,
        key_filename=args.key,
        timeout=args.timeout,
        auto_add_host=not args.no_auto_add
    ) as client:
        
        if not client.connected:
            sys.exit(1)
        
        # Save host key if requested
        if args.save_host_key:
            client.save_host_key()
        
        # Execute single command
        if args.command:
            stdout, stderr, status = client.execute_command(args.command, args.sudo)
            if stdout:
                print(stdout)
            if stderr:
                print(stderr, file=sys.stderr)
            sys.exit(status)
        
        # Upload file
        elif args.upload:
            client.open_sftp()
            client.upload_file(args.upload[0], args.upload[1])
        
        # Download file
        elif args.download:
            client.open_sftp()
            client.download_file(args.download[0], args.download[1])
        
        # List directory
        elif args.list:
            client.open_sftp()
            files = client.list_dir(args.list)
            for f in files:
                print(f)
        
        # Interactive shell
        elif args.shell:
            interactive_shell(client)
        
        # Simple connection test
        else:
            print(f"✅ Connected to {args.hostname}")
            host_key = client.get_host_key()
            if host_key:
                print(f"Host key: {host_key['type']} {host_key['fingerprint']}")
            
            # Get system info
            stdout, _, _ = client.execute_command("uname -a")
            if stdout:
                print(f"System: {stdout}")


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        # Interactive example
        print("=" * 60)
        print("🔌 PARAMIKO SSH CONNECTION EXAMPLE")
        print("=" * 60)
        
        # Get connection details
        hostname = input("Enter hostname: ").strip()
        username = input("Enter username: ").strip() or None
        use_key = input("Use SSH key? (y/n): ").lower() == 'y'
        
        key_file = None
        password = None
        
        if use_key:
            key_file = input("Enter path to private key: ").strip()
        else:
            password = getpass.getpass("Enter password: ")
        
        # Connect with auto host key addition
        print(f"\n🔑 Connecting to {hostname}...")
        
        with SSHClient(
            hostname=hostname,
            username=username,
            password=password,
            key_filename=key_file,
            auto_add_host=True
        ) as client:
            
            if client.connected:
                print("\n📋 Remote system info:")
                client.execute_command("uname -a")
                client.execute_command("df -h")
                
                # Get host key
                host_key = client.get_host_key()
                if host_key:
                    print(f"\n🔐 Host key fingerprint: {host_key['fingerprint']}")
                
                # Save host key
                save = input("\nSave host key to known_hosts? (y/n): ").lower()
                if save == 'y':
                    client.save_host_key()
            
            print("\n" + "=" * 60)
            print("Command line examples:")
            print(f"  python {sys.argv[0]} {hostname} -u {username} -c 'ls -la'")
            print(f"  python {sys.argv[0]} {hostname} -u {username} --shell")
            print(f"  python {sys.argv[0]} {hostname} -u {username} --upload local.txt remote.txt")
            print("=" * 60)