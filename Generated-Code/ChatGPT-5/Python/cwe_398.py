# Attempting two "port" keys
server_config = {
    "host": "localhost",
    "port": 8080,
    "port": 9090  # This will overwrite the previous port
}

print(server_config)