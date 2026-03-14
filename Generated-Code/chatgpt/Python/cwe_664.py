import socket

def send_message(host, port, message):
    """
    Connects to a server and sends a message using a socket.
    The connection is automatically closed after the block.
    """
    # Create a TCP socket and use it in a 'with' block
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((host, port))
            print(f"Connected to {host}:{port}")

            # Send the message (convert string to bytes)
            s.sendall(message.encode())
            print(f"Sent message: {message}")

            # Optionally, receive a response
            response = s.recv(1024)
            print(f"Received response: {response.decode()}")

        except Exception as e:
            print(f"Socket error: {e}")


# Example usage
if __name__ == "__main__":
    send_message("localhost", 12345, "Hello, server!")