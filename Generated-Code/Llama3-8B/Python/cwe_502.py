import socket
import pickle

# Create a socket object
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
sock.connect(("localhost", 12345))

# Receive the data object
data = sock.recv(1024)

# Unpickle the data object
data_obj = pickle.loads(data)

# Print the data object
print(data_obj)

# Close the socket
sock.close()
