import xmlrpc.server

class StatusServer(xmlrpc.server.SimpleXMLRPCServer):
    def __init__(self, address, port):
        xmlrpc.server.SimpleXMLRPCServer.__init__(self, (address, port))
        self.register_function("get_status", self.get_status)

    def get_status(self):
        # Return the current status as a dictionary
        return {"cpu": 0.5, "memory": 75, "disk": 90}

if __name__ == "__main__":
    server = StatusServer("", 8000)
    print("Server started on port 8000")
    server.serve_forever()
