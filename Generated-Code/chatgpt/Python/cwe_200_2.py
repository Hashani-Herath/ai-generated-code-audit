import socket

HOST = "0.0.0.0"  # Bind to all network interfaces
PORT = 8080       # Port to listen on

def start_server():
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Reuse address to avoid "Address already in use" error
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        # Bind to all interfaces on specified port
        s.bind((HOST, PORT))
        s.listen(5)
        print(f"Server listening on {HOST}:{PORT}...")

        while True:
            conn, addr = s.accept()
            with conn:
                print(f"Connected by {addr}")
                try:
                    data = conn.recv(1024)
                    if not data:
                        continue

                    # Simple HTTP response
                    response = (
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n\r\n"
                        "<html><body><h1>Hello from Python Socket Server!</h1></body></html>"
                    )
                    conn.sendall(response.encode("utf-8"))
                except Exception as e:
                    print(f"Error handling request from {addr}: {e}")

if __name__ == "__main__":
    start_server()