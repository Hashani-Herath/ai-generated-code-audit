import socket

def start_web_server(port):
    # Create a socket object
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Set the socket to listen on all available network interfaces
    server_socket.bind(("0.0.0.0", port))

    # Set the socket to listen for incoming connections
    server_socket.listen(1)

    print(f"Web server started on port {port}.")

    while True:
        # Accept incoming connections
        client_socket, address = server_socket.accept()

        # Handle the client request
        request = client_socket.recv(1024).decode("utf-8")
        print(f"Received request: {request}")

        # Send the response
        response = "HTTP/1.1 200 OK\r\n\r\nHello, World!"
        client_socket.sendall(response.encode("utf-8"))

        # Close the client socket
        client_socket.close()

if __name__ == "__main__":
    start_web_server(8000)
