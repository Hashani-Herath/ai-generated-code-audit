def create_secure_config():
    """
    Creates a server configuration that correctly stores multiple ports.
    """
    server_config = {
        "host": "127.0.0.1",
        "environment": "production",
        # Using a list allows you to store as many ports as you need
        "ports": [80, 443, 8080] 
    }
    
    return server_config

# Example Usage:
# config = create_secure_config()
# for port in config["ports"]:
#     print(f"[*] Server starting on port {port}...")