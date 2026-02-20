#!/usr/bin/env python3
"""
Network Socket Client - Connect and Send Message with Auto-closing
Uses context managers to automatically close connections
"""

import socket
import sys
import time
import ssl
from contextlib import closing, contextmanager
from typing import Optional, Union, Dict, Any
import argparse
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# METHOD 1: USING WITH STATEMENT (RECOMMENDED)
# ============================================

def send_message_with_context(host: str, port: int, message: str) -> bool:
    """
    Send a message using socket with context manager.
    Connection automatically closes when exiting the with block.
    
    Args:
        host: Server hostname or IP
        port: Server port
        message: Message to send
    
    Returns:
        True if successful
    """
    try:
        # Create socket and connect using context manager
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            # Set timeout to prevent hanging
            sock.settimeout(10)
            
            # Connect to server
            logger.info(f"Connecting to {host}:{port}...")
            sock.connect((host, port))
            
            # Send message
            logger.info(f"Sending: {message}")
            sock.sendall(message.encode('utf-8'))
            
            # Optional: receive response
            try:
                response = sock.recv(4096)
                if response:
                    logger.info(f"Received: {response.decode('utf-8')}")
            except socket.timeout:
                logger.warning("No response received (timeout)")
            
            # Connection automatically closed when exiting with block
        
        logger.info("Connection automatically closed")
        return True
        
    except socket.gaierror:
        logger.error(f"Hostname lookup failed: {host}")
        return False
    except socket.timeout:
        logger.error(f"Connection timeout to {host}:{port}")
        return False
    except ConnectionRefusedError:
        logger.error(f"Connection refused by {host}:{port}")
        return False
    except Exception as e:
        logger.error(f"Error: {e}")
        return False


# ============================================
# METHOD 2: USING CLOSING CONTEXT MANAGER
# ============================================

def send_message_with_closing(host: str, port: int, message: str) -> bool:
    """
    Send message using closing context manager from contextlib.
    """
    try:
        # Create socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        
        # Use closing context manager to ensure socket is closed
        with closing(sock):
            logger.info(f"Connecting to {host}:{port}...")
            sock.connect((host, port))
            
            logger.info(f"Sending: {message}")
            sock.sendall(message.encode('utf-8'))
            
            # Try to receive response
            try:
                response = sock.recv(4096)
                if response:
                    logger.info(f"Received: {response.decode('utf-8')}")
            except socket.timeout:
                pass
        
        logger.info("Connection automatically closed")
        return True
        
    except Exception as e:
        logger.error(f"Error: {e}")
        return False


# ============================================
# METHOD 3: CUSTOM CONTEXT MANAGER
# ============================================

@contextmanager
def socket_connection(host: str, port: int, timeout: int = 10):
    """
    Custom context manager for socket connections.
    
    Args:
        host: Server host
        port: Server port
        timeout: Socket timeout
    """
    sock = None
    try:
        # Create and connect socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        sock.connect((host, port))
        logger.debug(f"Socket connected to {host}:{port}")
        
        # Yield the socket for use in with block
        yield sock
        
    finally:
        # Automatically close socket when done
        if sock:
            sock.close()
            logger.debug("Socket automatically closed")


def send_message_custom_context(host: str, port: int, message: str) -> bool:
    """
    Send message using custom socket context manager.
    """
    try:
        with socket_connection(host, port) as sock:
            logger.info(f"Sending: {message}")
            sock.sendall(message.encode('utf-8'))
            
            # Receive response
            response = sock.recv(4096)
            if response:
                logger.info(f"Received: {response.decode('utf-8')}")
        
        return True
        
    except Exception as e:
        logger.error(f"Error: {e}")
        return False


# ============================================
# METHOD 4: SSL/TLS ENCRYPTED CONNECTION
# ============================================

def send_https_message(host: str, port: int = 443, message: str = "GET / HTTP/1.1\r\nHost: {host}\r\n\r\n") -> bool:
    """
    Send HTTPS message with automatic SSL context cleanup.
    """
    try:
        # Create SSL context
        context = ssl.create_default_context()
        
        # Connect with SSL using context manager
        with socket.create_connection((host, port)) as sock:
            with context.wrap_socket(sock, server_hostname=host) as ssock:
                logger.info(f"SSL connected to {host}:{port}")
                
                # Format message if it contains {host}
                if "{host}" in message:
                    message = message.format(host=host)
                
                logger.info(f"Sending: {message[:50]}...")
                ssock.sendall(message.encode('utf-8'))
                
                # Receive response
                response = ssock.recv(4096)
                if response:
                    logger.info(f"Received {len(response)} bytes")
                    logger.debug(f"Response preview: {response[:200].decode('utf-8')}")
        
        return True
        
    except Exception as e:
        logger.error(f"SSL connection error: {e}")
        return False


# ============================================
# METHOD 5: UDP CONNECTION (CONNECTIONLESS)
# ============================================

def send_udp_message(host: str, port: int, message: str) -> bool:
    """
    Send UDP message - no connection, but socket still auto-closes.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.settimeout(5)
            
            logger.info(f"Sending UDP message to {host}:{port}")
            sock.sendto(message.encode('utf-8'), (host, port))
            
            # Try to receive response (optional for UDP)
            try:
                data, server = sock.recvfrom(4096)
                logger.info(f"Received from {server}: {data.decode('utf-8')}")
            except socket.timeout:
                logger.info("No UDP response (timeout)")
        
        return True
        
    except Exception as e:
        logger.error(f"UDP error: {e}")
        return False


# ============================================
# METHOD 6: BROADCAST MESSAGE
# ============================================

def send_broadcast_message(port: int, message: str) -> bool:
    """
    Send broadcast message to all devices on network.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            # Enable broadcast
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            sock.settimeout(2)
            
            # Send to broadcast address
            broadcast_addr = ('<broadcast>', port)
            logger.info(f"Broadcasting to port {port}")
            sock.sendto(message.encode('utf-8'), broadcast_addr)
            
            # Listen for responses
            responses = []
            start_time = time.time()
            while time.time() - start_time < 3:
                try:
                    data, addr = sock.recvfrom(1024)
                    responses.append((addr, data.decode('utf-8')))
                    logger.info(f"Response from {addr}: {data.decode('utf-8')}")
                except socket.timeout:
                    break
            
            logger.info(f"Received {len(responses)} responses")
        
        return True
        
    except Exception as e:
        logger.error(f"Broadcast error: {e}")
        return False


# ============================================
# METHOD 7: MULTIPLE MESSAGES
# ============================================

def send_multiple_messages(host: str, port: int, messages: list) -> bool:
    """
    Send multiple messages over same connection.
    Connection auto-closes after all messages.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(10)
            sock.connect((host, port))
            
            for i, msg in enumerate(messages, 1):
                logger.info(f"Sending message {i}/{len(messages)}")
                sock.sendall(msg.encode('utf-8'))
                
                # Small delay between messages
                time.sleep(0.5)
                
                # Receive response
                try:
                    response = sock.recv(4096)
                    if response:
                        logger.info(f"Response {i}: {response.decode('utf-8')}")
                except socket.timeout:
                    logger.warning(f"No response for message {i}")
        
        return True
        
    except Exception as e:
        logger.error(f"Error: {e}")
        return False


# ============================================
# SIMPLE ECHO CLIENT
# ============================================

def echo_client(host: str = 'localhost', port: int = 9999, message: str = "Hello, Server!") -> bool:
    """
    Simple echo client - sends message and expects echo.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(5)
            sock.connect((host, port))
            
            logger.info(f"Connected to {host}:{port}")
            sock.sendall(message.encode('utf-8'))
            
            response = sock.recv(1024)
            logger.info(f"Echo received: {response.decode('utf-8')}")
            
        return True
        
    except Exception as e:
        logger.error(f"Echo client error: {e}")
        return False


# ============================================
# TEST ECHO SERVER (FOR DEMONSTRATION)
# ============================================

def run_echo_server(host: str = 'localhost', port: int = 9999):
    """
    Simple echo server for testing.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((host, port))
        server.listen(1)
        
        logger.info(f"Echo server listening on {host}:{port}")
        
        while True:
            try:
                client, addr = server.accept()
                with client:
                    logger.info(f"Client connected from {addr}")
                    data = client.recv(1024)
                    if data:
                        logger.info(f"Received: {data.decode('utf-8')}")
                        client.sendall(data)  # Echo back
                        logger.info("Echo sent")
            except KeyboardInterrupt:
                logger.info("Server shutting down")
                break
            except Exception as e:
                logger.error(f"Server error: {e}")


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Socket client with auto-closing connections')
    parser.add_argument('host', nargs='?', default='localhost', help='Server host')
    parser.add_argument('port', nargs='?', type=int, default=9999, help='Server port')
    parser.add_argument('-m', '--message', default='Hello, Server!', help='Message to send')
    parser.add_argument('-p', '--protocol', choices=['tcp', 'udp', 'ssl', 'broadcast'],
                       default='tcp', help='Protocol to use')
    parser.add_argument('--http', action='store_true', help='Send HTTP request')
    parser.add_argument('--multiple', nargs='+', help='Send multiple messages')
    parser.add_argument('--echo-server', action='store_true', help='Run echo server')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    if args.echo_server:
        run_echo_server(args.host, args.port)
        return
    
    if args.http:
        success = send_https_message(args.host, args.port or 80, 
                                     f"GET / HTTP/1.1\r\nHost: {args.host}\r\n\r\n")
    
    elif args.protocol == 'udp':
        success = send_udp_message(args.host, args.port, args.message)
    
    elif args.protocol == 'ssl':
        success = send_https_message(args.host, args.port or 443, args.message)
    
    elif args.protocol == 'broadcast':
        success = send_broadcast_message(args.port, args.message)
    
    elif args.multiple:
        success = send_multiple_messages(args.host, args.port, args.multiple)
    
    else:  # tcp
        success = send_message_with_context(args.host, args.port, args.message)
    
    sys.exit(0 if success else 1)


# ============================================
# DEMONSTRATION
# ============================================

def demo():
    """Demonstrate different socket communication methods."""
    
    print("=" * 70)
    print("🔌 NETWORK SOCKET DEMO - AUTO-CLOSING CONNECTIONS")
    print("=" * 70)
    
    # Start echo server in a separate thread for testing
    import threading
    
    server_thread = threading.Thread(target=run_echo_server, args=('localhost', 9999))
    server_thread.daemon = True
    server_thread.start()
    
    # Give server time to start
    time.sleep(1)
    
    # Method 1: Basic TCP with context manager
    print("\n1️⃣  TCP with context manager:")
    send_message_with_context('localhost', 9999, "Hello via TCP!")
    
    # Method 2: Using closing context manager
    print("\n2️⃣  Using closing context manager:")
    send_message_with_closing('localhost', 9999, "Hello via closing!")
    
    # Method 3: Custom context manager
    print("\n3️⃣  Custom context manager:")
    send_message_custom_context('localhost', 9999, "Hello via custom!")
    
    # Method 4: UDP (connectionless)
    print("\n4️⃣  UDP (connectionless):")
    send_udp_message('localhost', 9999, "Hello via UDP!")
    
    # Method 5: Multiple messages
    print("\n5️⃣  Multiple messages:")
    send_multiple_messages('localhost', 9999, ["First", "Second", "Third"])
    
    # Method 6: HTTP/HTTPS
    print("\n6️⃣  HTTPS (SSL):")
    send_https_message('example.com', 80, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n")
    
    print("\n" + "=" * 70)
    print("✅ All connections automatically closed")
    print("=" * 70)
    
    print("\n📝 QUICK REFERENCE:")
    print("""
    # TCP with auto-close
    with socket.socket() as sock:
        sock.connect((host, port))
        sock.send(message)
        # Auto-closed when exiting block
    
    # UDP with auto-close
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(message, (host, port))
        # Auto-closed when exiting block
    
    # SSL/TLS with auto-close
    with socket.create_connection((host, port)) as sock:
        with context.wrap_socket(sock) as ssock:
            ssock.send(message)
            # Both sockets auto-closed
    """)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo()