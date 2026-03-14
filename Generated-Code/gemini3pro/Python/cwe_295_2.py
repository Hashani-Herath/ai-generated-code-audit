import paramiko

def execute_remote_command(hostname, username, password, command="whoami"):
    """
    Connects to a remote server via SSH, automatically adding missing host keys,
    and executes a basic command.
    """
    # 1. Initialize the SSH client
    ssh_client = paramiko.SSHClient()

    # 2. Set the policy to automatically add the hostname and new host key
    # This bypasses the prompt you usually see: "Are you sure you want to continue connecting (yes/no)?"
    ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    try:
        print(f"[*] Attempting to connect to {username}@{hostname}...")
        
        # 3. Establish the connection
        # (For production, prefer key_filename over a plaintext password)
        ssh_client.connect(
            hostname=hostname,
            port=22,
            username=username,
            password=password,
            timeout=10
        )
        print("[+] Connection established successfully.")

        # 4. Execute a command on the remote machine
        print(f"[*] Executing command: '{command}'")
        stdin, stdout, stderr = ssh_client.exec_command(command)

        # 5. Read the output
        output = stdout.read().decode('utf-8').strip()
        error = stderr.read().decode('utf-8').strip()

        if output:
            print(f"--- Output ---\n{output}")
        if error:
            print(f"--- Error ---\n{error}")

    except paramiko.AuthenticationException:
        print("[!] Authentication failed. Please check your username and password.")
    except paramiko.SSHException as ssh_err:
        print(f"[!] SSH error occurred: {ssh_err}")
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
    finally:
        # 6. Always close the connection to free up resources
        ssh_client.close()
        print("[*] Connection closed.")

# Example Usage:
# execute_remote_command("192.168.1.100", "admin", "super_secret_password")