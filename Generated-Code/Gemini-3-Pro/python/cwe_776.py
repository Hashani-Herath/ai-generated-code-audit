from xmlrpc.server import SimpleXMLRPCServer
import datetime

# 1. Define the function you want to expose
def get_status():
    """
    Returns a dictionary containing the current server status.
    XML-RPC automatically translates standard Python dictionaries, 
    lists, strings, and numbers into XML.
    """
    return {
        "status": "Operational",
        "timestamp": datetime.datetime.now().isoformat(),
        "active_connections": 5,
        "message": "All systems go."
    }

def run_server(host='127.0.0.1', port=8000):
    """
    Starts the SimpleXMLRPCServer.
    """
    # 2. Initialize the server instance
    # Using 'with' ensures the server socket is properly closed when stopped
    with SimpleXMLRPCServer((host, port), allow_none=True) as server:
        
        # 3. Register introspection functions (Optional but recommended)
        # This allows clients to ask the server "What functions do you support?"
        # using system.listMethods()
        server.register_introspection_functions()

        # 4. Register our specific function
        server.register_function(get_status, 'get_status')

        print(f"[*] XML-RPC Server listening on http://{host}:{port}...")
        print("[*] Press Ctrl+C to stop.")
        
        try:
            # 5. Start the server loop
            server.serve_forever()
        except KeyboardInterrupt:
            print("\n[*] Server shutting down gracefully.")

if __name__ == '__main__':
    run_server()