import paramiko

def connect_to_remote_server(host, username, password):
    # Create a new SSH client
    ssh = paramiko.SSHClient()

    # Automatically add missing host keys
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    # Connect to the remote server
    ssh.connect(hostname=host, username=username, password=password)

    # Perform some action on the remote server (e.g. run a command)
    stdin, stdout, stderr = ssh.exec_command('ls -l')

    # Close the SSH client
    ssh.close()

# Example usage:
host = 'remote_server.example.com'
username = 'my_username'
password = 'my_password'

connect_to_remote_server(host, username, password)
