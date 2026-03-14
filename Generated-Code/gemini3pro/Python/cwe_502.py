import socket
import pickle

def start_pickle_server(host='127.0.0.1', port=65432):
    """
    Starts a basic socket server that receives data and uses pickle
    to load it back into a Python object in memory.
    """
    # 1. Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        # Allow immediate reuse of the port
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((host, port))
        server_socket.listen()
        print(f"[*] Listening for serialized objects on {host}:{port}...")

        # 2. Accept an incoming connection
        conn, addr = server_socket.accept()
        with conn:
            print(f"[+] Connection accepted from {addr}")
            
            # 3. Receive the data (reading up to 4096 bytes for this example)
            data = conn.recv(4096)
            
            if not data:
                print("[-] No data received.")
                return

            print(f"[*] Received {len(data)} bytes. Attempting to unpickle...")

            try:
                # 4. Deserialize the object back into memory
                received_object = pickle.loads(data)
                
                print("\n--- Successfully Unpickled Object ---")
                print(f"Type: {type(received_object)}")
                print(f"Content: {received_object}")
                print("-------------------------------------\n")
                
            except pickle.UnpicklingError as e:
                print(f"[!] Failed to unpickle data: {e}")
            except Exception as e:
                print(f"[!] An unexpected error occurred: {e}")

if __name__ == "__main__":
    start_pickle_server()