#!/usr/bin/env python3
"""
Socket-based Object Transfer using Pickle
Send and receive Python objects over network sockets
"""

import socket
import pickle
import struct
import threading
import time
import sys
import argparse
from typing import Any, Dict, List, Optional, Tuple
from datetime import datetime
import hashlib
import zlib
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# PICKLE SOCKET RECEIVER
# ============================================

class PickleSocketReceiver:
    """Receive and unpickle objects over socket"""
    
    def __init__(self, host: str = 'localhost', port: int = 9999):
        """
        Initialize receiver.
        
        Args:
            host: Host to bind to
            port: Port to listen on
        """
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
    
    def start_server(self):
        """Start the receiving server."""
        try:
            # Create socket
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # Bind and listen
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            self.running = True
            
            logger.info(f"📡 Server listening on {self.host}:{self.port}")
            
            # Accept connections
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    logger.info(f"🔌 Connection from {client_address}")
                    
                    # Handle client in new thread
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except KeyboardInterrupt:
                    break
                except Exception as e:
                    logger.error(f"Error accepting connection: {e}")
                    
        except Exception as e:
            logger.error(f"Server error: {e}")
        finally:
            self.stop()
    
    def stop(self):
        """Stop the server."""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
            logger.info("🛑 Server stopped")
    
    def handle_client(self, client_socket: socket.socket, client_address: Tuple[str, int]):
        """
        Handle a client connection.
        
        Args:
            client_socket: Client socket
            client_address: Client address
        """
        try:
            # Receive and unpickle objects
            while self.running:
                obj = self.receive_object(client_socket)
                
                if obj is None:
                    # Connection closed
                    break
                
                # Process received object
                self.process_received_object(obj, client_address)
                
        except Exception as e:
            logger.error(f"Error handling client {client_address}: {e}")
        finally:
            client_socket.close()
            logger.info(f"🔌 Connection closed for {client_address}")
    
    def receive_object(self, sock: socket.socket) -> Optional[Any]:
        """
        Receive a pickled object from socket.
        
        Protocol:
        - First 4 bytes: object size (big-endian)
        - Next 4 bytes: checksum (optional)
        - Then the pickled data
        
        Args:
            sock: Connected socket
        
        Returns:
            Unpickled object or None if connection closed
        """
        try:
            # Receive object size (4 bytes, big-endian)
            size_data = self.recv_all(sock, 4)
            if not size_data:
                return None
            
            obj_size = struct.unpack('!I', size_data)[0]
            
            # Receive checksum (4 bytes, optional)
            checksum_data = self.recv_all(sock, 4)
            if not checksum_data:
                return None
            
            expected_checksum = struct.unpack('!I', checksum_data)[0]
            
            # Receive pickled data
            data = self.recv_all(sock, obj_size)
            if not data:
                return None
            
            # Verify checksum
            actual_checksum = zlib.crc32(data) & 0xffffffff
            if actual_checksum != expected_checksum:
                logger.warning(f"Checksum mismatch! Expected {expected_checksum}, got {actual_checksum}")
                return None
            
            # Unpickle object
            obj = pickle.loads(data)
            
            logger.info(f"✅ Received object: {type(obj).__name__} ({obj_size} bytes)")
            return obj
            
        except socket.timeout:
            logger.warning("Socket timeout")
            return None
        except pickle.PickleError as e:
            logger.error(f"Pickle error: {e}")
            return None
        except Exception as e:
            logger.error(f"Receive error: {e}")
            return None
    
    def recv_all(self, sock: socket.socket, n: int) -> Optional[bytes]:
        """
        Receive exactly n bytes from socket.
        
        Args:
            sock: Connected socket
            n: Number of bytes to receive
        
        Returns:
            Bytes received or None if connection closed
        """
        data = bytearray()
        while len(data) < n:
            try:
                packet = sock.recv(n - len(data))
                if not packet:
                    return None
                data.extend(packet)
            except socket.timeout:
                continue
            except Exception as e:
                logger.error(f"Receive error: {e}")
                return None
        return bytes(data)
    
    def process_received_object(self, obj: Any, client_address: Tuple[str, int]):
        """
        Process received object.
        Override this method for custom processing.
        
        Args:
            obj: Received object
            client_address: Client address
        """
        # Default processing - just log object info
        logger.info(f"📦 Processing object from {client_address}")
        
        if isinstance(obj, dict):
            logger.info(f"   Dictionary with {len(obj)} items")
            for key, value in obj.items():
                logger.info(f"     {key}: {type(value).__name__}")
        
        elif isinstance(obj, (list, tuple)):
            logger.info(f"   {type(obj).__name__} with {len(obj)} items")
        
        elif hasattr(obj, '__dict__'):
            logger.info(f"   Custom object: {obj.__class__.__name__}")
            for attr, value in obj.__dict__.items():
                logger.info(f"     {attr}: {value}")
        
        else:
            logger.info(f"   Object value: {obj}")


# ============================================
# PICKLE SOCKET SENDER
# ============================================

class PickleSocketSender:
    """Send pickled objects over socket"""
    
    def __init__(self, host: str = 'localhost', port: int = 9999):
        """
        Initialize sender.
        
        Args:
            host: Receiver host
            port: Receiver port
        """
        self.host = host
        self.port = port
        self.socket = None
        self.connected = False
    
    def connect(self) -> bool:
        """Connect to receiver."""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.connected = True
            logger.info(f"✅ Connected to {self.host}:{self.port}")
            return True
            
        except Exception as e:
            logger.error(f"Connection failed: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from receiver."""
        if self.socket:
            self.socket.close()
            self.connected = False
            logger.info("🔌 Disconnected")
    
    def send_object(self, obj: Any) -> bool:
        """
        Send pickled object.
        
        Protocol:
        - First 4 bytes: object size (big-endian)
        - Next 4 bytes: checksum (CRC32)
        - Then the pickled data
        
        Args:
            obj: Object to send
        
        Returns:
            True if successful
        """
        if not self.connected:
            logger.error("Not connected")
            return False
        
        try:
            # Pickle object
            data = pickle.dumps(obj, protocol=pickle.HIGHEST_PROTOCOL)
            
            # Calculate checksum
            checksum = zlib.crc32(data) & 0xffffffff
            
            # Create header
            header = struct.pack('!II', len(data), checksum)
            
            # Send header + data
            self.socket.sendall(header + data)
            
            logger.info(f"📤 Sent {type(obj).__name__} ({len(data)} bytes)")
            return True
            
        except pickle.PickleError as e:
            logger.error(f"Pickle error: {e}")
            return False
        except Exception as e:
            logger.error(f"Send error: {e}")
            return False
    
    def send_objects(self, objects: List[Any]) -> Tuple[int, int]:
        """
        Send multiple objects.
        
        Args:
            objects: List of objects to send
        
        Returns:
            Tuple of (success_count, fail_count)
        """
        success = 0
        fail = 0
        
        for i, obj in enumerate(objects):
            if self.send_object(obj):
                success += 1
            else:
                fail += 1
            
            # Small delay between sends
            time.sleep(0.1)
        
        return success, fail


# ============================================
# EXAMPLE CUSTOM OBJECTS
# ============================================

class Person:
    """Example custom class for pickling"""
    
    def __init__(self, name: str, age: int, email: str = None):
        self.name = name
        self.age = age
        self.email = email
        self.created = datetime.now()
    
    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age}, email='{self.email}')"


class DataPacket:
    """Example data packet"""
    
    def __init__(self, packet_id: int, data: Any, metadata: Dict = None):
        self.id = packet_id
        self.data = data
        self.metadata = metadata or {}
        self.timestamp = time.time()
    
    def __repr__(self):
        return f"DataPacket(id={self.id}, data_type={type(self.data).__name__})"


# ============================================
# SIMPLE RECEIVER FUNCTION
# ============================================

def simple_receive(host: str = 'localhost', port: int = 9999):
    """
    Simple function to receive one object.
    
    Args:
        host: Host to bind to
        port: Port to listen on
    """
    receiver = PickleSocketReceiver(host, port)
    
    try:
        # Create socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((host, port))
        sock.listen(1)
        
        logger.info(f"Waiting for connection on {host}:{port}...")
        
        # Accept connection
        client_sock, addr = sock.accept()
        logger.info(f"Connected to {addr}")
        
        # Receive size
        size_data = client_sock.recv(4)
        if not size_data:
            logger.error("No data received")
            return
        
        obj_size = struct.unpack('!I', size_data)[0]
        
        # Receive checksum
        checksum_data = client_sock.recv(4)
        expected_checksum = struct.unpack('!I', checksum_data)[0]
        
        # Receive data
        data = b''
        while len(data) < obj_size:
            chunk = client_sock.recv(min(4096, obj_size - len(data)))
            if not chunk:
                break
            data += chunk
        
        # Verify checksum
        actual_checksum = zlib.crc32(data) & 0xffffffff
        if actual_checksum != expected_checksum:
            logger.error("Checksum mismatch!")
            return
        
        # Unpickle
        obj = pickle.loads(data)
        
        logger.info(f"✅ Received: {obj}")
        
        client_sock.close()
        sock.close()
        
    except Exception as e:
        logger.error(f"Error: {e}")


# ============================================
# SIMPLE SENDER FUNCTION
# ============================================

def simple_send(obj: Any, host: str = 'localhost', port: int = 9999):
    """
    Simple function to send one object.
    
    Args:
        obj: Object to send
        host: Receiver host
        port: Receiver port
    """
    sender = PickleSocketSender(host, port)
    
    if sender.connect():
        sender.send_object(obj)
        sender.disconnect()


# ============================================
# CLIENT/SERVER DEMO
# ============================================

def start_server(host: str = 'localhost', port: int = 9999):
    """Start receiver server."""
    receiver = PickleSocketReceiver(host, port)
    
    # Override process method for custom handling
    def custom_process(self, obj, addr):
        logger.info(f"🎯 Custom processing for object from {addr}")
        
        if isinstance(obj, Person):
            logger.info(f"   Person: {obj.name}, {obj.age} years old")
        
        elif isinstance(obj, DataPacket):
            logger.info(f"   Packet {obj.id}: {obj.data}")
        
        elif isinstance(obj, dict) and 'command' in obj:
            logger.info(f"   Command: {obj['command']}")
            if obj['command'] == 'shutdown':
                logger.info("Shutdown requested")
                self.running = False
    
    # Bind custom method
    receiver.process_received_object = lambda obj, addr: custom_process(receiver, obj, addr)
    
    try:
        receiver.start_server()
    except KeyboardInterrupt:
        receiver.stop()


def start_client(host: str = 'localhost', port: int = 9999):
    """Start client that sends various objects."""
    sender = PickleSocketSender(host, port)
    
    if not sender.connect():
        return
    
    try:
        # Send different types of objects
        objects = [
            # Simple types
            "Hello, World!",
            42,
            3.14159,
            [1, 2, 3, 4, 5],
            {"name": "Alice", "age": 30, "city": "New York"},
            
            # Custom objects
            Person("Bob", 25, "bob@example.com"),
            Person("Charlie", 35),
            
            # Complex objects
            DataPacket(1, {"temperature": 22.5, "humidity": 60}),
            DataPacket(2, [1, 1, 2, 3, 5, 8], {"type": "fibonacci"}),
            
            # Command objects
            {"command": "status", "timestamp": time.time()},
            {"command": "shutdown", "reason": "demo_complete"}
        ]
        
        for obj in objects:
            logger.info(f"\n📦 Sending: {obj}")
            sender.send_object(obj)
            time.sleep(1)  # Delay between sends
        
    finally:
        sender.disconnect()


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Pickle Socket Transfer')
    parser.add_argument('mode', choices=['server', 'client', 'send', 'receive'],
                       help='Mode: server (listen), client (send demo), send (send one), receive (receive one)')
    parser.add_argument('--host', default='localhost', help='Host address')
    parser.add_argument('--port', type=int, default=9999, help='Port number')
    parser.add_argument('--data', help='Data to send (for send mode)')
    
    args = parser.parse_args()
    
    if args.mode == 'server':
        logger.info(f"Starting server on {args.host}:{args.port}")
        start_server(args.host, args.port)
    
    elif args.mode == 'client':
        logger.info(f"Starting client connecting to {args.host}:{args.port}")
        start_client(args.host, args.port)
    
    elif args.mode == 'send':
        if not args.data:
            logger.error("Please provide data with --data")
            return
        
        # Try to eval the data (for dict/list) or use as string
        try:
            obj = eval(args.data)
        except:
            obj = args.data
        
        simple_send(obj, args.host, args.port)
    
    elif args.mode == 'receive':
        simple_receive(args.host, args.port)


# ============================================
# DEMO / TESTING
# ============================================

def demo():
    """Run a quick demo with subprocesses."""
    import subprocess
    import time
    
    logger.info("=" * 60)
    logger.info("PICKLE SOCKET TRANSFER DEMO")
    logger.info("=" * 60)
    
    # Start server in background
    server_process = subprocess.Popen(
        [sys.executable, __file__, 'server'],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    
    # Wait for server to start
    time.sleep(2)
    
    # Start client
    logger.info("\n🚀 Starting client...")
    client_process = subprocess.Popen(
        [sys.executable, __file__, 'client'],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    
    # Wait for client to finish
    client_process.wait()
    
    # Let server process shutdown
    time.sleep(2)
    server_process.terminate()
    
    # Show output
    logger.info("\n📋 Server Output:")
    stdout, stderr = server_process.communicate()
    if stdout:
        print(stdout.decode())
    if stderr:
        print(stderr.decode())


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo()