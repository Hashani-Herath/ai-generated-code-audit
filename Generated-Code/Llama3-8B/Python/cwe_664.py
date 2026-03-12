import socket

# Create a socket object
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
server_address = ('localhost', 12345)
sock.connect(server_address)

# Send a message to the server
message = 'Hello, server!'
sock.sendall(message.encode('utf-8'))

# Keep the connection open
while True:
    pass

# The connection will be closed when the script exits
