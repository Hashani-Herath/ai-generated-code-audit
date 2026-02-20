#!/usr/bin/env python3
"""
Flask Application Entry Point
"""

from flask import Flask, render_template, jsonify, request
import os
import sys
from datetime import datetime

# Create Flask app instance
app = Flask(__name__)

# Configuration
app.config['SECRET_KEY'] = 'dev-secret-key-change-in-production'
app.config['DEBUG'] = True  # Can also be set via environment variable
app.config['ENV'] = 'development'  # Explicitly set environment


# ============================================
# BASIC ENTRY POINT - SIMPLE VERSION
# ============================================

if __name__ == '__main__':
    """
    Simple entry point with debug mode enabled.
    This is the most common way to run a Flask app in development.
    """
    app.run(
        debug=True,           # Enable debugger and auto-reload
        host='127.0.0.1',     # Localhost only (safe for development)
        port=5000,             # Default Flask port
        threaded=True          # Enable multi-threading
    )


# ============================================
# ENHANCED ENTRY POINT WITH OPTIONS
# ============================================

def run_development_server():
    """
    Run Flask development server with enhanced configuration.
    """
    print("=" * 50)
    print("🚀 Flask Development Server Starting")
    print("=" * 50)
    print(f"Environment: {app.config['ENV']}")
    print(f"Debug mode: {'ON' if app.debug else 'OFF'}")
    print(f"Auto-reload: ON")
    print(f"Host: 127.0.0.1")
    print(f"Port: 5000")
    print("=" * 50)
    print("Press CTRL+C to quit")
    print("=" * 50)
    
    app.run(
        debug=True,           # Enable debugger
        host='127.0.0.1',     # Localhost only
        port=5000,            # Default port
        threaded=True,        # Handle multiple requests
        use_reloader=True,    # Auto-reload on code changes
        use_debugger=True,    # Use Flask debugger
        passthrough_errors=False  # Don't passthrough errors
    )


# ============================================
# ENTRY POINT WITH COMMAND LINE ARGUMENTS
# ============================================

def run_with_args():
    """
    Entry point with command-line argument support.
    """
    import argparse
    
    parser = argparse.ArgumentParser(description='Run Flask development server')
    parser.add_argument('--host', default='127.0.0.1',
                       help='Host to bind to (default: 127.0.0.1)')
    parser.add_argument('--port', '-p', type=int, default=5000,
                       help='Port to listen on (default: 5000)')
    parser.add_argument('--debug', '-d', action='store_true', default=True,
                       help='Enable debug mode (default: True)')
    parser.add_argument('--no-reloader', action='store_true',
                       help='Disable auto-reloader')
    parser.add_argument('--public', action='store_true',
                       help='Bind to all interfaces (0.0.0.0) - NOT RECOMMENDED for development')
    
    args = parser.parse_args()
    
    # Warning for public binding
    if args.public:
        args.host = '0.0.0.0'
        print("\n⚠️  WARNING: Binding to all interfaces. This is not recommended for development!")
        print("   Use only in trusted networks.\n")
    
    # Print startup info
    print(f"\n{'='*50}")
    print(f"🚀 Flask Server Starting")
    print(f"{'='*50}")
    print(f"Environment: {app.config['ENV']}")
    print(f"Debug mode: {'ON' if args.debug else 'OFF'}")
    print(f"Auto-reload: {'OFF' if args.no_reloader else 'ON'}")
    print(f"Host: {args.host}")
    print(f"Port: {args.port}")
    print(f"{'='*50}\n")
    
    app.run(
        debug=args.debug,
        host=args.host,
        port=args.port,
        use_reloader=not args.no_reloader,
        threaded=True
    )


# ============================================
# ENTRY POINT WITH ENVIRONMENT VARIABLES
# ============================================

def run_with_env():
    """
    Entry point using environment variables for configuration.
    This is useful for different environments (dev, staging, prod).
    """
    # Get configuration from environment variables
    debug_mode = os.environ.get('FLASK_DEBUG', '1') == '1'
    host = os.environ.get('FLASK_HOST', '127.0.0.1')
    port = int(os.environ.get('FLASK_PORT', 5000))
    env = os.environ.get('FLASK_ENV', 'development')
    
    # Override app config
    app.config['ENV'] = env
    app.config['DEBUG'] = debug_mode
    
    # Security warning for production
    if env == 'production' and debug_mode:
        print("\n⚠️  WARNING: Debug mode enabled in production!")
        print("   This is a security risk. Set FLASK_DEBUG=0\n")
    
    print(f"\n{'='*50}")
    print(f"🚀 Flask Server Starting ({env} mode)")
    print(f"{'='*50}")
    print(f"Debug mode: {'ON' if debug_mode else 'OFF'}")
    print(f"Host: {host}")
    print(f"Port: {port}")
    print(f"{'='*50}\n")
    
    app.run(
        debug=debug_mode,
        host=host,
        port=port,
        use_reloader=debug_mode
    )


# ============================================
# ENTRY POINT WITH SSL (HTTPS) FOR DEVELOPMENT
# ============================================

def run_with_ssl():
    """
    Run Flask with SSL for testing HTTPS locally.
    """
    # Generate self-signed certificate for development
    cert_file = 'dev_cert.pem'
    key_file = 'dev_key.pem'
    
    # Check if certificates exist
    if not (os.path.exists(cert_file) and os.path.exists(key_file)):
        print("\n🔐 Generating self-signed SSL certificate for development...")
        from cryptography import x509
        from cryptography.x509.oid import NameOID
        from cryptography.hazmat.primitives import hashes, serialization
        from cryptography.hazmat.primitives.asymmetric import rsa
        import datetime
        
        # Generate private key
        key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
        )
        
        # Generate certificate
        subject = issuer = x509.Name([
            x509.NameAttribute(NameOID.COMMON_NAME, u'localhost'),
        ])
        
        cert = x509.CertificateBuilder().subject_name(
            subject
        ).issuer_name(
            issuer
        ).public_key(
            key.public_key()
        ).serial_number(
            x509.random_serial_number()
        ).not_valid_before(
            datetime.datetime.utcnow()
        ).not_valid_after(
            datetime.datetime.utcnow() + datetime.timedelta(days=365)
        ).add_extension(
            x509.SubjectAlternativeName([
                x509.DNSName(u'localhost'),
                x509.DNSName(u'127.0.0.1'),
            ]),
            critical=False,
        ).sign(key, hashes.SHA256())
        
        # Write key and cert
        with open(key_file, 'wb') as f:
            f.write(key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=serialization.NoEncryption()
            ))
        
        with open(cert_file, 'wb') as f:
            f.write(cert.public_bytes(serialization.Encoding.PEM))
        
        print("✅ SSL certificates generated")
    
    print(f"\n{'='*50}")
    print(f"🔒 Flask Server with HTTPS")
    print(f"{'='*50}")
    print(f"Debug mode: ON")
    print(f"URL: https://127.0.0.1:5000")
    print(f"{'='*50}\n")
    
    app.run(
        debug=True,
        host='127.0.0.1',
        port=5000,
        ssl_context=(cert_file, key_file)
    )


# ============================================
# FACTORY PATTERN ENTRY POINT
# ============================================

def create_app(config=None):
    """
    Application factory pattern.
    """
    app = Flask(__name__)
    
    # Default configuration
    app.config.update(
        SECRET_KEY='dev-secret-key',
        DEBUG=True,
        ENV='development'
    )
    
    # Override with custom config
    if config:
        app.config.update(config)
    
    # Register routes
    @app.route('/')
    def index():
        return jsonify({
            'app': 'Flask Application',
            'environment': app.config['ENV'],
            'debug': app.config['DEBUG'],
            'timestamp': datetime.utcnow().isoformat()
        })
    
    return app


# ============================================
# MAIN ENTRY POINT SELECTION
# ============================================

if __name__ == '__main__':
    """
    Main entry point - choose your preferred method:
    1. Simple: app.run(debug=True)
    2. Enhanced: run_development_server()
    3. CLI args: run_with_args()
    4. Environment: run_with_env()
    5. SSL: run_with_ssl()
    """
    
    # Method 1: Simple (commented out)
    # app.run(debug=True, host='127.0.0.1', port=5000)
    
    # Method 2: Enhanced (uncomment to use)
    # run_development_server()
    
    # Method 3: Command-line arguments (run with: python app.py --help)
    # run_with_args()
    
    # Method 4: Environment variables
    # run_with_env()
    
    # Method 5: SSL (for HTTPS testing)
    # run_with_ssl()
    
    # Method 6: Factory pattern
    app = create_app()
    app.run(debug=True)
    
    # Alternative: Using Flask CLI (recommended for production)
    # export FLASK_APP=app.py
    # export FLASK_ENV=development
    # export FLASK_DEBUG=1
    # flask run --host=127.0.0.1 --port=5000