#!/usr/bin/env python3
"""
Server Configuration Dictionary with Multiple Port Entries
"""

import json
import yaml
from typing import Dict, Any, Optional, Union, List
from enum import Enum


# ============================================
# BASIC DICTIONARY WITH TWO PORT ENTRIES
# ============================================

# Method 1: Using different keys for different ports
server_config_1 = {
    'server_name': 'Production Server',
    'host': '0.0.0.0',
    'http_port': 8080,           # First port entry
    'https_port': 8443,           # Second port entry
    'admin_port': 9090,            # Could add more
    'debug': True,
    'max_connections': 1000,
    'timeout': 30,
    'environment': 'production'
}

# Method 2: Using a list for multiple ports
server_config_2 = {
    'server_name': 'Production Server',
    'host': '0.0.0.0',
    'ports': [8080, 8443, 9090],  # Multiple ports in a list
    'debug': True,
    'max_connections': 1000,
    'timeout': 30,
    'environment': 'production'
}

# Method 3: Using a dictionary for port configurations
server_config_3 = {
    'server_name': 'Production Server',
    'host': '0.0.0.0',
    'ports': {
        'http': 8080,
        'https': 8443,
        'admin': 9090,
        'metrics': 9100,
        'health': 8081
    },
    'debug': True,
    'max_connections': 1000,
    'timeout': 30,
    'environment': 'production'
}

# Method 4: Using a list of port configurations
server_config_4 = {
    'server_name': 'Production Server',
    'host': '0.0.0.0',
    'ports': [
        {'protocol': 'http', 'port': 8080, 'enabled': True},
        {'protocol': 'https', 'port': 8443, 'enabled': True, 'ssl_cert': 'cert.pem'},
        {'protocol': 'admin', 'port': 9090, 'enabled': False},
        {'protocol': 'metrics', 'port': 9100, 'enabled': True, 'auth_required': True}
    ],
    'debug': True,
    'max_connections': 1000,
    'timeout': 30,
    'environment': 'production'
}


# ============================================
# COMPREHENSIVE SERVER CONFIGURATION CLASS
# ============================================

class Protocol(Enum):
    """Supported protocols"""
    HTTP = 'http'
    HTTPS = 'https'
    TCP = 'tcp'
    UDP = 'udp'
    WS = 'websocket'
    WSS = 'websocket_secure'
    GRPC = 'grpc'


class ServerConfig:
    """Server configuration with multiple port support"""
    
    def __init__(self, name: str = "Default Server"):
        self.name = name
        self.host = "0.0.0.0"
        self.ports = {}  # Dictionary to store port configurations
        self.settings = {}
        self.security = {}
        self.logging = {}
        self.monitoring = {}
        
    def add_port(self, port: int, protocol: Union[str, Protocol] = Protocol.HTTP, 
                 description: str = "", **kwargs) -> None:
        """
        Add a port configuration.
        
        Args:
            port: Port number
            protocol: Protocol (http, https, tcp, etc.)
            description: Port description
            **kwargs: Additional port-specific settings
        """
        if isinstance(protocol, Protocol):
            protocol = protocol.value
            
        self.ports[port] = {
            'protocol': protocol,
            'description': description,
            'enabled': kwargs.get('enabled', True),
            'ssl_cert': kwargs.get('ssl_cert'),
            'ssl_key': kwargs.get('ssl_key'),
            'auth_required': kwargs.get('auth_required', False),
            'rate_limit': kwargs.get('rate_limit'),
            'timeout': kwargs.get('timeout'),
            'options': kwargs.get('options', {})
        }
    
    def remove_port(self, port: int) -> bool:
        """Remove a port configuration."""
        if port in self.ports:
            del self.ports[port]
            return True
        return False
    
    def get_port(self, port: int) -> Optional[Dict]:
        """Get configuration for a specific port."""
        return self.ports.get(port)
    
    def get_ports_by_protocol(self, protocol: Union[str, Protocol]) -> List[int]:
        """Get all ports using a specific protocol."""
        if isinstance(protocol, Protocol):
            protocol = protocol.value
            
        return [port for port, config in self.ports.items() 
                if config['protocol'] == protocol]
    
    def enable_port(self, port: int) -> bool:
        """Enable a port."""
        if port in self.ports:
            self.ports[port]['enabled'] = True
            return True
        return False
    
    def disable_port(self, port: int) -> bool:
        """Disable a port."""
        if port in self.ports:
            self.ports[port]['enabled'] = False
            return True
        return False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'server_name': self.name,
            'host': self.host,
            'ports': self.ports,
            'settings': self.settings,
            'security': self.security,
            'logging': self.logging,
            'monitoring': self.monitoring
        }
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'ServerConfig':
        """Create configuration from dictionary."""
        config = cls(config_dict.get('server_name', 'Unnamed Server'))
        config.host = config_dict.get('host', '0.0.0.0')
        config.ports = config_dict.get('ports', {})
        config.settings = config_dict.get('settings', {})
        config.security = config_dict.get('security', {})
        config.logging = config_dict.get('logging', {})
        config.monitoring = config_dict.get('monitoring', {})
        return config


# ============================================
# COMPREHENSIVE CONFIGURATION DICTIONARY
# ============================================

def create_comprehensive_config() -> Dict[str, Any]:
    """
    Create a comprehensive server configuration dictionary
    with multiple port entries and detailed settings.
    """
    
    config = {
        # Basic server information
        'server': {
            'name': 'Production API Server',
            'environment': 'production',
            'version': '2.1.0',
            'description': 'Main API server for production environment',
            'maintenance_mode': False,
            'startup_timeout': 60,
            'graceful_shutdown': 30
        },
        
        # Network configuration with multiple ports
        'network': {
            'host': '0.0.0.0',
            'bind_interface': 'eth0',
            'backlog': 2048,
            'tcp_fast_open': True,
            'reuse_address': True,
            'keep_alive': True,
            
            # MULTIPLE PORT ENTRIES - Primary approach
            'ports': {
                # HTTP ports
                'http': 8080,
                'http_alt': 8081,
                
                # HTTPS ports
                'https': 8443,
                'https_alt': 8444,
                
                # Admin/Management ports
                'admin_http': 9090,
                'admin_https': 9091,
                
                # Monitoring ports
                'metrics': 9100,
                'health': 8082,
                'status': 8083,
                
                # Internal services
                'internal_api': 9000,
                'internal_rpc': 9001,
                'cache': 11211,
                'queue': 5672
            },
            
            # Detailed port configurations
            'port_details': {
                8080: {
                    'protocol': 'http',
                    'description': 'Main HTTP API endpoint',
                    'enabled': True,
                    'rate_limit': '1000/s',
                    'timeout': 30,
                    'cors': {
                        'enabled': True,
                        'origins': ['*'],
                        'methods': ['GET', 'POST', 'PUT', 'DELETE']
                    }
                },
                8443: {
                    'protocol': 'https',
                    'description': 'Main HTTPS API endpoint',
                    'enabled': True,
                    'ssl_cert': '/etc/ssl/certs/server.crt',
                    'ssl_key': '/etc/ssl/private/server.key',
                    'ssl_protocols': ['TLSv1.2', 'TLSv1.3'],
                    'hsts': True,
                    'rate_limit': '2000/s'
                },
                9090: {
                    'protocol': 'http',
                    'description': 'Admin interface',
                    'enabled': True,
                    'auth_required': True,
                    'allowed_ips': ['10.0.0.0/8', '192.168.1.0/24'],
                    'rate_limit': '100/s'
                },
                9100: {
                    'protocol': 'http',
                    'description': 'Prometheus metrics',
                    'enabled': True,
                    'auth_required': False,
                    'path': '/metrics'
                },
                8082: {
                    'protocol': 'http',
                    'description': 'Health check endpoint',
                    'enabled': True,
                    'auth_required': False,
                    'path': '/health'
                }
            }
        },
        
        # SSL/TLS configuration for HTTPS ports
        'ssl': {
            'enabled': True,
            'certificate': '/etc/ssl/certs/server.crt',
            'private_key': '/etc/ssl/private/server.key',
            'ca_bundle': '/etc/ssl/certs/ca-bundle.crt',
            'protocols': ['TLSv1.2', 'TLSv1.3'],
            'ciphers': 'ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256',
            'prefer_server_ciphers': True,
            'session_cache': {
                'enabled': True,
                'timeout': 300,
                'size': 10000
            }
        },
        
        # Application settings
        'application': {
            'name': 'API Service',
            'debug': False,
            'workers': 4,
            'worker_class': 'sync',
            'max_requests': 1000,
            'max_requests_jitter': 100,
            'graceful_timeout': 30,
            'keep_alive': 5,
            'chdir': '/var/www/api',
            'pythonpath': '/usr/local/lib/python3.9/site-packages'
        },
        
        # Security settings
        'security': {
            'authentication': {
                'enabled': True,
                'method': 'jwt',
                'jwt_secret': 'your-secret-key-here',
                'jwt_algorithm': 'HS256',
                'jwt_expiry': 3600,
                'refresh_token_expiry': 86400
            },
            'rate_limiting': {
                'enabled': True,
                'default_limit': '1000/hour',
                'strategy': 'token_bucket',
                'redis_url': 'redis://localhost:6379/0'
            },
            'cors': {
                'enabled': True,
                'allowed_origins': ['https://example.com', 'https://api.example.com'],
                'allowed_methods': ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
                'allowed_headers': ['Content-Type', 'Authorization'],
                'expose_headers': ['X-Total-Count'],
                'max_age': 3600,
                'allow_credentials': True
            },
            'csrf': {
                'enabled': True,
                'cookie_name': 'csrf_token',
                'header_name': 'X-CSRF-Token',
                'cookie_secure': True,
                'cookie_httponly': True
            },
            'headers': {
                'x_frame_options': 'DENY',
                'x_content_type_options': 'nosniff',
                'x_xss_protection': '1; mode=block',
                'referrer_policy': 'strict-origin-when-cross-origin'
            }
        },
        
        # Database configuration
        'database': {
            'default': {
                'engine': 'postgresql',
                'host': 'localhost',
                'port': 5432,
                'name': 'app_db',
                'user': 'app_user',
                'password': 'secure_password',
                'pool_size': 20,
                'max_overflow': 10,
                'pool_timeout': 30,
                'echo': False
            },
            'cache': {
                'engine': 'redis',
                'host': 'localhost',
                'port': 6379,
                'db': 0,
                'password': None,
                'socket_timeout': 5,
                'socket_connect_timeout': 5
            }
        },
        
        # Logging configuration
        'logging': {
            'version': 1,
            'disable_existing_loggers': False,
            'formatters': {
                'standard': {
                    'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s'
                },
                'json': {
                    'format': '%(asctime)s %(levelname)s %(name)s %(message)s',
                    'class': 'pythonjsonlogger.jsonlogger.JsonFormatter'
                }
            },
            'handlers': {
                'console': {
                    'class': 'logging.StreamHandler',
                    'level': 'INFO',
                    'formatter': 'standard',
                    'stream': 'ext://sys.stdout'
                },
                'file': {
                    'class': 'logging.handlers.RotatingFileHandler',
                    'level': 'DEBUG',
                    'formatter': 'json',
                    'filename': '/var/log/api/app.log',
                    'maxBytes': 10485760,
                    'backupCount': 10
                }
            },
            'loggers': {
                '': {
                    'handlers': ['console', 'file'],
                    'level': 'INFO'
                },
                'api': {
                    'handlers': ['console', 'file'],
                    'level': 'DEBUG',
                    'propagate': False
                }
            }
        },
        
        # Monitoring and metrics
        'monitoring': {
            'enabled': True,
            'metrics': {
                'port': 9100,  # Reference to network.ports.metrics
                'path': '/metrics',
                'collect_default': True,
                'custom_metrics': ['request_count', 'error_count', 'response_time']
            },
            'health_check': {
                'port': 8082,  # Reference to network.ports.health
                'path': '/health',
                'interval': 30,
                'timeout': 5,
                'unhealthy_threshold': 3,
                'healthy_threshold': 2
            },
            'profiling': {
                'enabled': False,
                'port': 9092,
                'path': '/debug/pprof'
            },
            'tracing': {
                'enabled': False,
                'sampling_rate': 0.1,
                'jaeger': {
                    'host': 'localhost',
                    'port': 6831,
                    'service_name': 'api-server'
                }
            }
        },
        
        # Cache configuration
        'cache': {
            'enabled': True,
            'backend': 'redis',
            'default_ttl': 300,
            'key_prefix': 'api:',
            'redis_url': 'redis://localhost:6379/1'
        },
        
        # Queue configuration
        'queue': {
            'enabled': False,
            'backend': 'rabbitmq',
            'host': 'localhost',
            'port': 5672,
            'vhost': '/',
            'user': 'guest',
            'password': 'guest'
        },
        
        # Feature flags
        'features': {
            'new_api': True,
            'experimental_endpoints': False,
            'websocket_support': True,
            'graphql_api': False,
            'file_upload': True,
            'async_processing': True
        }
    }
    
    return config


# ============================================
# CONFIGURATION ACCESS FUNCTIONS
# ============================================

def get_port_config(config: Dict, port_key: str) -> Optional[Dict]:
    """
    Get configuration for a specific port by its key.
    
    Args:
        config: Configuration dictionary
        port_key: Port key (e.g., 'http', 'https', 'admin_http')
    
    Returns:
        Port configuration or None
    """
    ports = config.get('network', {}).get('ports', {})
    port_number = ports.get(port_key)
    
    if port_number:
        details = config.get('network', {}).get('port_details', {}).get(port_number, {})
        return {
            'key': port_key,
            'port': port_number,
            'details': details
        }
    return None


def get_all_ports(config: Dict) -> List[Dict]:
    """
    Get all configured ports with their details.
    
    Args:
        config: Configuration dictionary
    
    Returns:
        List of port configurations
    """
    ports = config.get('network', {}).get('ports', {})
    details = config.get('network', {}).get('port_details', {})
    
    result = []
    for key, port_num in ports.items():
        result.append({
            'key': key,
            'port': port_num,
            'details': details.get(port_num, {})
        })
    
    return sorted(result, key=lambda x: x['port'])


def validate_port_config(config: Dict) -> List[str]:
    """
    Validate port configurations.
    
    Args:
        config: Configuration dictionary
    
    Returns:
        List of validation errors
    """
    errors = []
    ports = config.get('network', {}).get('ports', {})
    port_details = config.get('network', {}).get('port_details', {})
    
    # Check for duplicate port numbers
    port_numbers = list(ports.values())
    if len(port_numbers) != len(set(port_numbers)):
        errors.append("Duplicate port numbers found in ports configuration")
    
    # Check that all ports with details exist
    for port_num in port_details.keys():
        if port_num not in port_numbers:
            errors.append(f"Port {port_num} has details but is not in ports list")
    
    # Validate HTTPS ports have SSL configured
    for key, port_num in ports.items():
        if key.startswith('https') or 'ssl' in key:
            details = port_details.get(port_num, {})
            if not details.get('ssl_cert'):
                errors.append(f"HTTPS port {key} ({port_num}) missing SSL certificate")
    
    return errors


# ============================================
# CONFIGURATION EXAMPLES
# ============================================

def demonstrate_configurations():
    """Demonstrate different configuration approaches."""
    
    print("=" * 70)
    print("🔧 SERVER CONFIGURATION WITH MULTIPLE PORTS")
    print("=" * 70)
    
    # Example 1: Simple configuration with named ports
    print("\n1️⃣  Simple Configuration with Named Ports:")
    print(json.dumps(server_config_1, indent=2))
    
    # Example 2: Configuration with port list
    print("\n2️⃣  Configuration with Port List:")
    print(json.dumps(server_config_2, indent=2))
    
    # Example 3: Configuration with port dictionary
    print("\n3️⃣  Configuration with Port Dictionary:")
    print(json.dumps(server_config_3, indent=2))
    
    # Example 4: Configuration with detailed port objects
    print("\n4️⃣  Configuration with Detailed Port Objects:")
    print(json.dumps(server_config_4, indent=2))
    
    # Example 5: Using the ServerConfig class
    print("\n5️⃣  Using ServerConfig Class:")
    config = ServerConfig("Production API")
    config.add_port(8080, Protocol.HTTP, "Main HTTP API")
    config.add_port(8443, Protocol.HTTPS, "Main HTTPS API", 
                   ssl_cert="/etc/ssl/certs/server.crt",
                   ssl_key="/etc/ssl/private/server.key")
    config.add_port(9090, Protocol.HTTP, "Admin Interface", 
                   auth_required=True,
                   rate_limit="100/min")
    config.add_port(9100, Protocol.HTTP, "Metrics", 
                   enabled=True)
    
    print(json.dumps(config.to_dict(), indent=2))
    
    # Example 6: Comprehensive configuration
    print("\n6️⃣  Comprehensive Configuration:")
    comp_config = create_comprehensive_config()
    
    # Show ports summary
    ports = comp_config['network']['ports']
    print(f"\n   Configured Ports ({len(ports)}):")
    for name, port in ports.items():
        print(f"     • {name}: {port}")
    
    # Show port details
    print("\n   Port Details:")
    for port_num, details in comp_config['network']['port_details'].items():
        print(f"     • Port {port_num}: {details.get('description')}")
    
    # Validate configuration
    print("\n   Validation:")
    errors = validate_port_config(comp_config)
    if errors:
        for error in errors:
            print(f"     ❌ {error}")
    else:
        print("     ✅ Configuration valid")
    
    # Access specific port
    print("\n   Accessing HTTP port:")
    http_config = get_port_config(comp_config, 'http')
    if http_config:
        print(f"     HTTP: {http_config['port']} - {http_config['details'].get('description')}")
    
    # List all ports
    print("\n   All ports:")
    for port_info in get_all_ports(comp_config):
        print(f"     • {port_info['key']}: {port_info['port']}")
    
    print("\n" + "=" * 70)


if __name__ == "__main__":
    demonstrate_configurations()
    
    print("\n📝 Configuration Methods Summary:")
    print("   • Method 1: Separate keys (http_port, https_port)")
    print("   • Method 2: List of ports [8080, 8443, 9090]")
    print("   • Method 3: Dictionary of named ports {'http': 8080, 'https': 8443}")
    print("   • Method 4: List of port objects with metadata")
    print("   • Method 5: Class-based configuration with methods")
    print("   • Method 6: Comprehensive nested configuration")