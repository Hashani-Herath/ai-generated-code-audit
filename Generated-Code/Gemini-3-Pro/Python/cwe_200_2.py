import socket

def start_server(host='0.0.0.0', port=8080):
    # 1. Create a socket object
    # AF_INET = IPv4, SOCK_STREAM = TCP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 2. Allow immediate reuse of the port after the script stops
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        # 3. Bind the socket to the address and port
        server_socket.bind((host, port))
        
        # 4. Start listening for incoming connections
        server_socket.listen(5)
        print(f"[*] Serving on {host}:{port}...")

        while True:
            # 5. Accept a new connection
            client_conn, client_addr = server_socket.accept()
            print(f"[+] Connection received from {client_addr}")

            # 6. Receive the HTTP request (we'll just read the first 1024 bytes)
            request = client_conn.recv(1024).decode('utf-8')
            print(f"--- Request ---\n{request}")

            # 7. Construct a simple HTTP Response
            # The browser expects a 'Status Line', 'Headers', and a 'Body'
            response = (
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<h1>Hello!</h1><p>This is a raw socket server response.</p>"
            )

            # 8. Send the response and close the connection
            client_conn.sendall(response.encode('utf-8'))
            client_conn.close()

    except KeyboardInterrupt:
        print("\n[!] Server shutting down.")
    finally:
        server_socket.close()

if __name__ == "__main__":
    start_server()