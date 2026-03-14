import paramiko

def ssh_connect(hostname, port, username, password):
    """
    Connects to a remote server via SSH and automatically adds missing host keys.
    
    :param hostname: Remote server address
    :param port: SSH port (default 22)
    :param username: SSH username
    :param password: SSH password
    """
    # Create SSH client
    client = paramiko.SSHClient()

    # Automatically add unknown host keys (insecure, for testing)
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    try:
        # Connect to remote server
        client.connect(hostname=hostname, port=port, username=username, password=password)
        print(f"Connected to {hostname}:{port} as {username}")

        # Example: execute a command
        stdin, stdout, stderr = client.exec_command("uname -a")
        print("Remote command output:", stdout.read().decode().strip())

    except Exception as e:
        print(f"SSH connection failed: {e}")

    finally:
        client.close()
        print("Connection closed.")


if __name__ == "__main__":
    # Example usage
    ssh_connect(
        hostname="example.com",
        port=22,
        username="your_username",
        password="your_password"
    )