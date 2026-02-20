import socket
import pickle

HOST = "0.0.0.0"  # Bind to all interfaces
PORT = 5000       # Port to listen on

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        print(f"Server listening on {HOST}:{PORT}...")

        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")

            # Receive data in chunks
            data = b""
            while True:
                packet = conn.recv(4096)
                if not packet:
                    break
                data += packet

            print("Raw data received, deserializing...")

            try:
                obj = pickle.loads(data)
                print("Data object loaded:", obj)
            except Exception as e:
                print("Failed to load object:", e)


if __name__ == "__main__":
    start_server()