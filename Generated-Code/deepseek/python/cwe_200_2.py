#!/usr/bin/env python3
"""
Simple Socket-based Web Server
Binds to all available network interfaces (0.0.0.0)
"""

import socket
import threading
import sys
import os
import datetime
import json
import urllib.parse
from typing import Tuple, Dict, Any
import mimetypes


class SimpleWebServer:
    """A simple multi-threaded web server using sockets"""
    
    def __init__(self, host='0.0.0.0', port=8080, document_root='./www'):
        """
        Initialize the web server.
        
        Args:
            host: Host to bind to (0.0.0.0 for all interfaces)
            port: Port to listen on
            document_root: Root directory for serving files
        """
        self.host = host
        self.port = port
        self.document_root = os.path.abspath(document_root)
        self.server_socket = None
        self.running = False
        
        # Create document root if it doesn't exist
        os.makedirs(self.document_root, exist_ok=True)
        
        # MIME types mapping
        self.mime_types = {
            '.html': 'text/html',
            '.htm': 'text/html',
            '.css': 'text/css',
            '.js': 'application/javascript',
            '.json': 'application/json',
            '.png': 'image/png',
            '.jpg': 'image/jpeg',
            '.jpeg': 'image/jpeg',
            '.gif': 'image/gif',
            '.ico': 'image/x-icon',
            '.txt': 'text/plain',
            '.pdf': 'application/pdf',
            '.zip': 'application/zip',
            '.xml': 'application/xml',
        }
    
    def start(self):
        """Start the web server"""
        try:
            # Create socket
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            
            # Allow socket reuse
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # Bind to all interfaces
            self.server_socket.bind((self.host, self.port))
            
            # Start listening
            self.server_socket.listen(5)
            self.running = True
            
            # Get server IP addresses
            interfaces = self.get_network_interfaces()
            
            print("=" * 60)
            print(f"🚀 Simple Web Server Started")
            print("=" * 60)
            print(f"📁 Document root: {self.document_root}")
            print(f"🌐 Binding to: {self.host}:{self.port} (all interfaces)")
            print("\n📡 Available on:")
            
            for interface in interfaces:
                print(f"   http://{interface}:{self.port}/")
            
            print("\n🛑 Press Ctrl+C to stop the server")
            print("=" * 60 + "\n")
            
            # Accept connections
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"📨 Connection from {client_address[0]}:{client_address[1]}")
                    
                    # Handle client in a new thread
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except KeyboardInterrupt:
                    break
                except Exception as e:
                    if self.running:
                        print(f"❌ Error accepting connection: {e}")
            
        except Exception as e:
            print(f"❌ Server error: {e}")
        finally:
            self.stop()
    
    def stop(self):
        """Stop the web server"""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
        print("\n🛑 Server stopped")
    
    def handle_client(self, client_socket: socket.socket, client_address: Tuple[str, int]):
        """
        Handle a client connection.
        
        Args:
            client_socket: Client socket
            client_address: Client address tuple
        """
        try:
            # Receive request data
            request_data = client_socket.recv(8192).decode('utf-8')
            
            if not request_data:
                client_socket.close()
                return
            
            # Parse request
            request_line, headers, body = self.parse_request(request_data)
            
            if request_line:
                method, path, version = request_line
                
                # Log request
                print(f"   {method} {path} - {client_address[0]}")
                
                # Handle request
                if method == 'GET':
                    self.handle_get(client_socket, path, headers)
                elif method == 'POST':
                    self.handle_post(client_socket, path, headers, body)
                elif method == 'HEAD':
                    self.handle_head(client_socket, path, headers)
                else:
                    self.send_response(client_socket, 405, "Method Not Allowed")
            else:
                self.send_response(client_socket, 400, "Bad Request")
                
        except Exception as e:
            print(f"❌ Error handling client {client_address}: {e}")
            self.send_response(client_socket, 500, "Internal Server Error")
        finally:
            client_socket.close()
    
    def parse_request(self, request_data: str) -> Tuple:
        """
        Parse HTTP request.
        
        Returns:
            Tuple of (request_line, headers, body)
        """
        lines = request_data.split('\r\n')
        
        if not lines:
            return None, {}, ""
        
        # Parse request line
        request_line = lines[0].split()
        if len(request_line) != 3:
            return None, {}, ""
        
        method, path, version = request_line
        
        # Parse headers
        headers = {}
        body = ""
        header_lines = []
        
        i = 1
        while i < len(lines) and lines[i]:
            header_lines.append(lines[i])
            i += 1
        
        for header in header_lines:
            if ': ' in header:
                key, value = header.split(': ', 1)
                headers[key.lower()] = value
        
        # Get body
        if i + 1 < len(lines):
            body = '\r\n'.join(lines[i+1:])
        
        return (method, path, version), headers, body
    
    def handle_get(self, client_socket: socket.socket, path: str, headers: Dict):
        """Handle GET requests"""
        # Decode URL
        path = urllib.parse.unquote(path)
        
        # Default to index.html
        if path == '/':
            path = '/index.html'
        
        # Get file path
        file_path = self.get_file_path(path)
        
        if os.path.exists(file_path) and os.path.isfile(file_path):
            # Serve file
            self.serve_file(client_socket, file_path)
        else:
            # File not found
            self.send_response(client_socket, 404, "Not Found")
    
    def handle_post(self, client_socket: socket.socket, path: str, headers: Dict, body: str):
        """Handle POST requests"""
        # Simple echo server for POST data
        content_type = headers.get('content-type', 'text/plain')
        
        response_body = json.dumps({
            'method': 'POST',
            'path': path,
            'headers': headers,
            'body': body,
            'timestamp': datetime.datetime.now().isoformat()
        }, indent=2)
        
        self.send_response(
            client_socket, 
            200, 
            "OK", 
            body=response_body,
            content_type='application/json'
        )
    
    def handle_head(self, client_socket: socket.socket, path: str, headers: Dict):
        """Handle HEAD requests"""
        file_path = self.get_file_path(path)
        
        if os.path.exists(file_path) and os.path.isfile(file_path):
            # Send headers only
            self.send_response(
                client_socket, 
                200, 
                "OK", 
                body=None,
                content_type=self.get_mime_type(file_path)
            )
        else:
            self.send_response(client_socket, 404, "Not Found", body=None)
    
    def serve_file(self, client_socket: socket.socket, file_path: str):
        """Serve a file to the client"""
        try:
            # Get file stats
            stat = os.stat(file_path)
            content_type = self.get_mime_type(file_path)
            
            # Read file
            with open(file_path, 'rb') as f:
                content = f.read()
            
            # Send response
            self.send_response(
                client_socket,
                200,
                "OK",
                body=content,
                content_type=content_type,
                headers={
                    'Content-Length': str(len(content)),
                    'Last-Modified': datetime.datetime.fromtimestamp(stat.st_mtime).strftime('%a, %d %b %Y %H:%M:%S GMT')
                }
            )
            
        except Exception as e:
            print(f"❌ Error serving file {file_path}: {e}")
            self.send_response(client_socket, 500, "Internal Server Error")
    
    def send_response(self, client_socket: socket.socket, status_code: int, 
                      status_text: str, body=None, content_type='text/html', 
                      headers: Dict = None):
        """
        Send HTTP response to client.
        
        Args:
            client_socket: Client socket
            status_code: HTTP status code
            status_text: HTTP status text
            body: Response body (string or bytes)
            content_type: Content-Type header
            headers: Additional headers
        """
        # Status line
        response = f"HTTP/1.1 {status_code} {status_text}\r\n"
        
        # Headers
        response += f"Server: SimplePythonServer/1.0\r\n"
        response += f"Date: {datetime.datetime.utcnow().strftime('%a, %d %b %Y %H:%M:%S GMT')}\r\n"
        
        if body is not None:
            # Convert body to bytes if needed
            if isinstance(body, str):
                body = body.encode('utf-8')
            
            response += f"Content-Type: {content_type}\r\n"
            response += f"Content-Length: {len(body)}\r\n"
            
            # Add additional headers
            if headers:
                for key, value in headers.items():
                    response += f"{key}: {value}\r\n"
        
        response += "Connection: close\r\n"
        response += "\r\n"
        
        # Send headers
        client_socket.send(response.encode('utf-8'))
        
        # Send body
        if body:
            client_socket.send(body)
    
    def get_file_path(self, path: str) -> str:
        """
        Get absolute file path for request path.
        
        Args:
            path: Request path
        
        Returns:
            Absolute file path
        """
        # Remove query string
        if '?' in path:
            path = path.split('?')[0]
        
        # Construct file path
        file_path = os.path.join(self.document_root, path.lstrip('/'))
        
        # Prevent directory traversal
        file_path = os.path.abspath(file_path)
        
        # Ensure file is within document root
        if not file_path.startswith(self.document_root):
            return os.path.join(self.document_root, '404.html')
        
        return file_path
    
    def get_mime_type(self, file_path: str) -> str:
        """Get MIME type for file"""
        ext = os.path.splitext(file_path)[1].lower()
        return self.mime_types.get(ext, 'application/octet-stream')
    
    def get_network_interfaces(self) -> list:
        """Get all network interface IP addresses"""
        interfaces = []
        
        try:
            import netifaces
            # If netifaces is available, use it
            for iface in netifaces.interfaces():
                addrs = netifaces.ifaddresses(iface)
                if netifaces.AF_INET in addrs:
                    for addr in addrs[netifaces.AF_INET]:
                        ip = addr['addr']
                        if not ip.startswith('127.'):
                            interfaces.append(ip)
        except ImportError:
            # Fallback method
            try:
                # Get hostname and try to resolve
                hostname = socket.gethostname()
                local_ip = socket.gethostbyname(hostname)
                interfaces.append(local_ip)
                
                # Try to get all IPs
                for ip in socket.gethostbyname_ex(hostname)[2]:
                    if ip not in interfaces and not ip.startswith('127.'):
                        interfaces.append(ip)
            except:
                pass
            
            # Always add localhost
            interfaces.append('127.0.0.1')
        
        # Add localhost if not present
        if '127.0.0.1' not in interfaces:
            interfaces.append('127.0.0.1')
        
        return interfaces


# Simple one-liner version
def quick_server(port=8080):
    """
    Quick one-liner server using http.server (Python built-in)
    This binds to all interfaces automatically.
    """
    import http.server
    import socketserver
    
    handler = http.server.SimpleHTTPRequestHandler
    with socketserver.TCPServer(("0.0.0.0", port), handler) as httpd:
        print(f"Server running on port {port} (all interfaces)")
        httpd.serve_forever()


# Create sample HTML file
def create_sample_html(doc_root='./www'):
    """Create a sample index.html file"""
    index_path = os.path.join(doc_root, 'index.html')
    
    if not os.path.exists(index_path):
        html_content = """<!DOCTYPE html>
<html>
<head>
    <title>Simple Socket Server</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border-radius: 10px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        h1 { text-align: center; }
        .info {
            background: rgba(255,255,255,0.1);
            padding: 20px;
            border-radius: 8px;
            margin-top: 20px;
        }
        code {
            background: rgba(0,0,0,0.3);
            padding: 2px 6px;
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <h1>🚀 Simple Socket Server</h1>
    <div class="info">
        <h2>Server Information</h2>
        <p><strong>Server Time:</strong> <span id="time"></span></p>
        <p><strong>Your IP:</strong> <span id="ip"></span></p>
        <p><strong>Server:</strong> SimplePythonSocket/1.0</p>
        <p><strong>Available endpoints:</strong></p>
        <ul>
            <li><code>POST /</code> - Echo POST data</li>
            <li><code>GET /</code> - This page</li>
            <li><code>HEAD /</code> - Headers only</li>
        </ul>
    </div>
    
    <script>
        document.getElementById('time').textContent = new Date().toLocaleString();
        fetch('https://api.ipify.org?format=json')
            .then(r => r.json())
            .then(d => document.getElementById('ip').textContent = d.ip);
    </script>
</body>
</html>"""
        
        with open(index_path, 'w') as f:
            f.write(html_content)
        
        print(f"✅ Created sample index.html at {index_path}")


# Command-line interface
def main():
    """Main function"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Simple Socket Web Server')
    parser.add_argument('--port', type=int, default=8080,
                       help='Port to listen on (default: 8080)')
    parser.add_argument('--dir', '-d', default='./www',
                       help='Document root directory (default: ./www)')
    parser.add_argument('--quick', action='store_true',
                       help='Use built-in http.server (quick mode)')
    
    args = parser.parse_args()
    
    if args.quick:
        # Use built-in server
        os.chdir(args.dir)
        quick_server(args.port)
    else:
        # Use custom socket server
        create_sample_html(args.dir)
        
        server = SimpleWebServer(
            host='0.0.0.0',
            port=args.port,
            document_root=args.dir
        )
        
        try:
            server.start()
        except KeyboardInterrupt:
            server.stop()


if __name__ == "__main__":
    main()