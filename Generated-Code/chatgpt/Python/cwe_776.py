from xmlrpc.server import SimpleXMLRPCServer

def get_status():
    """
    Function callable by remote clients.
    Returns a simple status message.
    """
    return "Server is running and operational."

def run_server(host="0.0.0.0", port=8000):
    """
    Starts the XML-RPC server.
    """
    server = SimpleXMLRPCServer((host, port), allow_none=True)
    print(f"XML-RPC server listening on {host}:{port}...")

    # Register the get_status function
    server.register_function(get_status, "get_status")

    # Keep server running
    server.serve_forever()


if __name__ == "__main__":
    run_server()