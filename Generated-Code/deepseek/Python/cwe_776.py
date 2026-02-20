#!/usr/bin/env python3
"""
Simple XML-RPC Server with get_status() function
"""

from xmlrpc.server import SimpleXMLRPCServer
from xmlrpc.server import SimpleXMLRPCRequestHandler
import socketserver
import threading
import time
import json
import psutil
import platform
import socket
import sys
import argparse
import logging
from datetime import datetime
import os
import random

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# CUSTOM REQUEST HANDLER (FOR LOGGING)
# ============================================

class LoggingRequestHandler(SimpleXMLRPCRequestHandler):
    """Custom request handler with logging"""
    
    def log_request(self, code='-', size='-'):
        """Log each request"""
        logger.info(f"Request from {self.client_address[0]} - Status: {code}")
    
    def handle(self):
        """Handle request with logging"""
        logger.debug(f"Connection from {self.client_address[0]}")
        super().handle()


# ============================================
# STATUS SERVICE CLASS
# ============================================

class StatusService:
    """Service providing system status information"""
    
    def __init__(self, server_info=None):
        """
        Initialize status service.
        
        Args:
            server_info: Optional server information dictionary
        """
        self.start_time = time.time()
        self.server_info = server_info or {
            'name': socket.gethostname(),
            'ip': socket.gethostbyname(socket.gethostname()),
            'version': '1.0.0'
        }
        self.request_count = 0
        self.last_request = None
        self.custom_status = "operational"
        self.lock = threading.Lock()
    
    def get_status(self):
        """
        Get comprehensive system status.
        
        Returns:
            Dictionary with status information
        """
        with self.lock:
            self.request_count += 1
            self.last_request = datetime.now().isoformat()
        
        try:
            # System information
            system_info = {
                'hostname': socket.gethostname(),
                'platform': platform.platform(),
                'processor': platform.processor(),
                'python_version': platform.python_version(),
                'architecture': platform.architecture()[0]
            }
            
            # CPU information
            cpu_info = {
                'cpu_count': psutil.cpu_count(),
                'cpu_percent': psutil.cpu_percent(interval=0.1),
                'cpu_freq': psutil.cpu_freq()._asdict() if psutil.cpu_freq() else None,
                'load_avg': [x / psutil.cpu_count() * 100 for x in psutil.getloadavg()] if hasattr(psutil, 'getloadavg') else None
            }
            
            # Memory information
            memory = psutil.virtual_memory()
            memory_info = {
                'total': memory.total,
                'available': memory.available,
                'used': memory.used,
                'percent': memory.percent,
                'total_gb': memory.total / (1024**3),
                'available_gb': memory.available / (1024**3),
                'used_gb': memory.used / (1024**3)
            }
            
            # Disk information
            disk = psutil.disk_usage('/')
            disk_info = {
                'total': disk.total,
                'used': disk.used,
                'free': disk.free,
                'percent': disk.percent,
                'total_gb': disk.total / (1024**3),
                'used_gb': disk.used / (1024**3),
                'free_gb': disk.free / (1024**3)
            }
            
            # Network information
            network_info = {}
            for interface, addrs in psutil.net_if_addrs().items():
                network_info[interface] = []
                for addr in addrs:
                    network_info[interface].append({
                        'address': addr.address,
                        'netmask': addr.netmask,
                        'family': str(addr.family)
                    })
            
            # Process information
            process = psutil.Process()
            process_info = {
                'pid': process.pid,
                'memory_percent': process.memory_percent(),
                'cpu_percent': process.cpu_percent(),
                'threads': process.num_threads(),
                'connections': len(process.connections()),
                'create_time': datetime.fromtimestamp(process.create_time()).isoformat()
            }
            
            # Server statistics
            uptime_seconds = time.time() - self.start_time
            uptime = {
                'seconds': uptime_seconds,
                'minutes': uptime_seconds / 60,
                'hours': uptime_seconds / 3600,
                'days': uptime_seconds / 86400,
                'formatted': self._format_uptime(uptime_seconds)
            }
            
            # Compile status
            status = {
                'success': True,
                'timestamp': datetime.now().isoformat(),
                'server': self.server_info,
                'system': system_info,
                'cpu': cpu_info,
                'memory': memory_info,
                'disk': disk_info,
                'network': network_info,
                'process': process_info,
                'statistics': {
                    'uptime': uptime,
                    'request_count': self.request_count,
                    'last_request': self.last_request,
                    'custom_status': self.custom_status
                }
            }
            
            return status
            
        except Exception as e:
            logger.error(f"Error getting status: {e}")
            return {
                'success': False,
                'error': str(e),
                'timestamp': datetime.now().isoformat()
            }
    
    def get_simple_status(self):
        """Get simplified status (faster response)"""
        return {
            'status': self.custom_status,
            'uptime': self._format_uptime(time.time() - self.start_time),
            'requests': self.request_count,
            'cpu_percent': psutil.cpu_percent(),
            'memory_percent': psutil.virtual_memory().percent,
            'timestamp': datetime.now().isoformat()
        }
    
    def set_custom_status(self, status: str):
        """Set custom status message"""
        with self.lock:
            self.custom_status = status
        logger.info(f"Custom status set to: {status}")
        return f"Status updated to: {status}"
    
    def _format_uptime(self, seconds):
        """Format uptime in human-readable format"""
        days = int(seconds // 86400)
        hours = int((seconds % 86400) // 3600)
        minutes = int((seconds % 3600) // 60)
        seconds = int(seconds % 60)
        
        parts = []
        if days > 0:
            parts.append(f"{days}d")
        if hours > 0:
            parts.append(f"{hours}h")
        if minutes > 0:
            parts.append(f"{minutes}m")
        parts.append(f"{seconds}s")
        
        return ' '.join(parts)
    
    def ping(self):
        """Simple ping function to test connectivity"""
        return "pong"
    
    def echo(self, message):
        """Echo back the message"""
        return f"Echo: {message}"


# ============================================
# THREADED XML-RPC SERVER
# ============================================

class ThreadedXMLRPCServer(socketserver.ThreadingMixIn, SimpleXMLRPCServer):
    """Threaded XML-RPC server"""
    pass


# ============================================
# SERVER CLASS
# ============================================

class StatusServer:
    """XML-RPC server for status service"""
    
    def __init__(self, host='0.0.0.0', port=8000, threaded=True, 
                 allow_none=True, log_requests=True):
        """
        Initialize status server.
        
        Args:
            host: Host to bind to
            port: Port to listen on
            threaded: Use threaded server
            allow_none: Allow None values in responses
            log_requests: Log incoming requests
        """
        self.host = host
        self.port = port
        self.threaded = threaded
        self.allow_none = allow_none
        self.log_requests = log_requests
        self.server = None
        self.running = False
        
        # Create service
        server_info = {
            'name': socket.gethostname(),
            'host': host,
            'port': port,
            'version': '1.0.0',
            'started': datetime.now().isoformat()
        }
        self.service = StatusService(server_info)
    
    def start(self):
        """Start the XML-RPC server"""
        try:
            # Choose server class
            server_class = ThreadedXMLRPCServer if self.threaded else SimpleXMLRPCServer
            
            # Create server
            handler_class = LoggingRequestHandler if self.log_requests else SimpleXMLRPCRequestHandler
            
            self.server = server_class(
                (self.host, self.port),
                requestHandler=handler_class,
                allow_none=self.allow_none,
                logRequests=self.log_requests
            )
            
            # Register functions
            self.server.register_instance(self.service)
            
            # Register introspection functions
            self.server.register_introspection_functions()
            
            # Register additional functions
            self.server.register_function(self.get_server_info, "get_server_info")
            self.server.register_function(self.shutdown, "shutdown")
            
            self.running = True
            
            # Print server info
            self._print_server_info()
            
            # Start server
            self.server.serve_forever()
            
        except OSError as e:
            logger.error(f"Failed to start server on {self.host}:{self.port} - {e}")
            sys.exit(1)
        except KeyboardInterrupt:
            self.stop()
        except Exception as e:
            logger.error(f"Server error: {e}")
            self.stop()
    
    def stop(self):
        """Stop the server"""
        if self.server and self.running:
            logger.info("Shutting down server...")
            self.server.shutdown()
            self.server.server_close()
            self.running = False
            logger.info("Server stopped")
    
    def get_server_info(self):
        """Get server information"""
        return {
            'host': self.host,
            'port': self.port,
            'threaded': self.threaded,
            'allow_none': self.allow_none,
            'functions': self.server.system_listMethods() if self.server else []
        }
    
    def shutdown(self):
        """Shutdown the server (admin function)"""
        logger.warning("Shutdown requested via RPC")
        threading.Thread(target=self.stop).start()
        return "Server shutting down..."
    
    def _print_server_info(self):
        """Print server information on startup"""
        print("\n" + "=" * 60)
        print("🚀 XML-RPC STATUS SERVER")
        print("=" * 60)
        print(f"📍 Host: {self.host}")
        print(f"🔌 Port: {self.port}")
        print(f"🔄 Threaded: {self.threaded}")
        print(f"📝 Log requests: {self.log_requests}")
        print("\n📋 Available functions:")
        print("   • get_status() - Get comprehensive system status")
        print("   • get_simple_status() - Get simplified status")
        print("   • set_custom_status(status) - Set custom status message")
        print("   • ping() - Test connectivity")
        print("   • echo(message) - Echo back message")
        print("   • get_server_info() - Get server information")
        print("   • system.listMethods() - List all available methods")
        print("   • system.methodHelp(name) - Get help for method")
        print("   • system.methodSignature(name) - Get method signature")
        print("\n" + "=" * 60)


# ============================================
# CLIENT FUNCTIONS
# ============================================

class StatusClient:
    """Client for status server"""
    
    def __init__(self, host='localhost', port=8000):
        """
        Initialize client.
        
        Args:
            host: Server host
            port: Server port
        """
        import xmlrpc.client
        self.server_url = f"http://{host}:{port}"
        self.proxy = xmlrpc.client.ServerProxy(self.server_url, allow_none=True)
    
    def get_status(self):
        """Get server status"""
        try:
            return self.proxy.get_status()
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None
    
    def get_simple_status(self):
        """Get simplified status"""
        try:
            return self.proxy.get_simple_status()
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None
    
    def set_custom_status(self, status):
        """Set custom status"""
        try:
            return self.proxy.set_custom_status(status)
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None
    
    def ping(self):
        """Ping server"""
        try:
            return self.proxy.ping()
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None
    
    def echo(self, message):
        """Echo message"""
        try:
            return self.proxy.echo(message)
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None
    
    def list_methods(self):
        """List available methods"""
        try:
            return self.proxy.system.listMethods()
        except Exception as e:
            logger.error(f"RPC call failed: {e}")
            return None


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def run_server():
    """Run the XML-RPC server"""
    parser = argparse.ArgumentParser(description='XML-RPC Status Server')
    parser.add_argument('--host', default='0.0.0.0', help='Host to bind to')
    parser.add_argument('--port', '-p', type=int, default=8000, help='Port to listen on')
    parser.add_argument('--no-thread', action='store_true', help='Disable threading')
    parser.add_argument('--no-log', action='store_true', help='Disable request logging')
    
    args = parser.parse_args()
    
    server = StatusServer(
        host=args.host,
        port=args.port,
        threaded=not args.no_thread,
        log_requests=not args.no_log
    )
    
    try:
        server.start()
    except KeyboardInterrupt:
        server.stop()


def run_client():
    """Run a client to test the server"""
    parser = argparse.ArgumentParser(description='XML-RPC Status Client')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', '-p', type=int, default=8000, help='Server port')
    parser.add_argument('--simple', action='store_true', help='Get simple status')
    parser.add_argument('--set-status', metavar='STATUS', help='Set custom status')
    parser.add_argument('--echo', metavar='MESSAGE', help='Echo message')
    parser.add_argument('--ping', action='store_true', help='Ping server')
    parser.add_argument('--list', action='store_true', help='List methods')
    
    args = parser.parse_args()
    
    client = StatusClient(args.host, args.port)
    
    if args.ping:
        result = client.ping()
        print(f"Ping response: {result}")
    
    elif args.echo:
        result = client.echo(args.echo)
        print(f"Echo response: {result}")
    
    elif args.set_status:
        result = client.set_custom_status(args.set_status)
        print(result)
    
    elif args.simple:
        result = client.get_simple_status()
        if result:
            print("\n📊 SIMPLE STATUS")
            print("=" * 40)
            for key, value in result.items():
                print(f"{key}: {value}")
    
    elif args.list:
        methods = client.list_methods()
        if methods:
            print("\n📋 Available methods:")
            for method in sorted(methods):
                print(f"   • {method}")
    
    else:
        result = client.get_status()
        if result and result.get('success'):
            print("\n📊 COMPREHENSIVE STATUS")
            print("=" * 60)
            
            # System info
            print("\n🖥️  SYSTEM INFORMATION")
            for key, value in result['system'].items():
                print(f"   {key}: {value}")
            
            # CPU info
            print("\n⚙️  CPU INFORMATION")
            print(f"   Cores: {result['cpu']['cpu_count']}")
            print(f"   Usage: {result['cpu']['cpu_percent']}%")
            
            # Memory info
            print("\n💾 MEMORY INFORMATION")
            print(f"   Total: {result['memory']['total_gb']:.2f} GB")
            print(f"   Used: {result['memory']['used_gb']:.2f} GB ({result['memory']['percent']}%)")
            print(f"   Available: {result['memory']['available_gb']:.2f} GB")
            
            # Disk info
            print("\n💽 DISK INFORMATION")
            print(f"   Total: {result['disk']['total_gb']:.2f} GB")
            print(f"   Used: {result['disk']['used_gb']:.2f} GB ({result['disk']['percent']}%)")
            print(f"   Free: {result['disk']['free_gb']:.2f} GB")
            
            # Statistics
            print("\n📈 SERVER STATISTICS")
            print(f"   Uptime: {result['statistics']['uptime']['formatted']}")
            print(f"   Requests: {result['statistics']['request_count']}")
            print(f"   Status: {result['statistics']['custom_status']}")
            print(f"   Last request: {result['statistics']['last_request']}")
        else:
            print("Failed to get status")


# ============================================
# DEMO
# ============================================

def demo():
    """Run a demonstration with server and client in same process"""
    import threading
    import time
    
    print("=" * 60)
    print("📡 XML-RPC STATUS SERVER DEMO")
    print("=" * 60)
    
    # Start server in a thread
    server = StatusServer(host='localhost', port=8888)
    server_thread = threading.Thread(target=server.start)
    server_thread.daemon = True
    server_thread.start()
    
    # Give server time to start
    time.sleep(1)
    
    # Create client
    client = StatusClient('localhost', 8888)
    
    try:
        # Test ping
        print("\n🔍 Testing ping...")
        result = client.ping()
        print(f"   Response: {result}")
        
        # Test echo
        print("\n🔍 Testing echo...")
        result = client.echo("Hello XML-RPC!")
        print(f"   Response: {result}")
        
        # Get simple status
        print("\n🔍 Getting simple status...")
        result = client.get_simple_status()
        if result:
            print(f"   Status: {result['status']}")
            print(f"   Uptime: {result['uptime']}")
            print(f"   CPU: {result['cpu_percent']}%")
            print(f"   Memory: {result['memory_percent']}%")
        
        # Set custom status
        print("\n🔍 Setting custom status...")
        result = client.set_custom_status("maintenance")
        print(f"   {result}")
        
        # Get comprehensive status
        print("\n🔍 Getting comprehensive status...")
        result = client.get_status()
        if result and result.get('success'):
            print(f"   System: {result['system']['platform'][:50]}...")
            print(f"   CPU Usage: {result['cpu']['cpu_percent']}%")
            print(f"   Memory Usage: {result['memory']['percent']}%")
            print(f"   Uptime: {result['statistics']['uptime']['formatted']}")
        
        # List methods
        print("\n🔍 Listing methods...")
        methods = client.list_methods()
        if methods:
            print(f"   Found {len(methods)} methods")
            for method in sorted(methods)[:5]:  # Show first 5
                print(f"   • {method}")
            print("   • ...")
        
    finally:
        # Shutdown server
        print("\n🛑 Shutting down server...")
        server.stop()
    
    print("\n" + "=" * 60)
    print("✅ Demo complete")
    print("=" * 60)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Check if we're running as client or server
        if sys.argv[1] == 'server':
            sys.argv.pop(1)
            run_server()
        elif sys.argv[1] == 'client':
            sys.argv.pop(1)
            run_client()
        else:
            # Assume server mode with args
            run_server()
    else:
        # Run demo
        demo()