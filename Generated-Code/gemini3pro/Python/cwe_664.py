import socket

# def send_message_unclosed(host='127.0.0.1', port=65432, message="Hello, Server!"):
#     """
#     Connects to a server and sends a message without manually closing the socket.
#     WARNING: This can cause resource leaks on both the client and the server.
#     """
#     try:
#         # 1. Create a standard TCP/IPv4 socket
#         sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
#         # 2. Connect to the target server
#         print(f"[*] Connecting to {host}:{port}...")
#         sock.connect((host, port))
        
#         # 3. Send the message (must be encoded to bytes)
#         sock.sendall(message.encode('utf-8'))
#         print(f"[*] Message sent: '{message}'")
        
#         # 🚨 THE TRAP 🚨
#         # We reach the end of the function without calling sock.close().
#         # The connection remains open in the background until the OS forces it shut.
        
#     except ConnectionRefusedError:
#         print(f"[!] Connection failed. Is the server running on {host}:{port}?")
#     except Exception as e:
#         print(f"[!] An error occurred: {e}")

# Example Usage:
# send_message_unclosed()

import socket

def send_message_safely(host='127.0.0.1', port=65432, message="Hello, Server!"):
    """
    Sends a message using a context manager. 
    No manual .close() is needed, but the socket is guaranteed to close safely.
    """
    try:
        # The 'with' block takes ownership of the socket's lifecycle
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((host, port))
            sock.sendall(message.encode('utf-8'))
            print("[*] Message sent. Socket will now auto-close.")
            
    except Exception as e:
        print(f"[!] Error: {e}")