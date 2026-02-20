#!/usr/bin/env python3
"""
Flask Application with SECRET_KEY Configuration
"""

from flask import Flask, session, jsonify, request
import os
import secrets
import string
from datetime import timedelta


# ============================================
# METHOD 1: DIRECT INITIALIZATION (SIMPLEST)
# ============================================

app = Flask(__name__)
app.secret_key = "dev-key-123"  # Simple string initialization


# ============================================
# METHOD 2: USING app.config DICTIONARY
# ============================================

app = Flask(__name__)
app.config['SECRET_KEY'] = "dev-key-123"


# ============================================
# METHOD 3: WITH CONFIGURATION CLASS
# ============================================

class Config:
    """Base configuration class"""
    SECRET_KEY = "dev-key-123"
    DEBUG = True
    TESTING = False


class DevelopmentConfig(Config):
    """Development configuration"""
    DEBUG = True
    SECRET_KEY = "dev-key-123"  # Development key
    SESSION_COOKIE_NAME = 'dev_session'
    SESSION_COOKIE_SECURE = False  # Allow HTTP in development
    SESSION_COOKIE_HTTPONLY = True
    SESSION_COOKIE_SAMESITE = 'Lax'
    PERMANENT_SESSION_LIFETIME = timedelta(days=31)


class ProductionConfig(Config):
    """Production configuration"""
    DEBUG = False
    SECRET_KEY = os.environ.get('SECRET_KEY', secrets.token_hex(32))
    SESSION_COOKIE_SECURE = True  # HTTPS only
    SESSION_COOKIE_HTTPONLY = True
    SESSION_COOKIE_SAMESITE = 'Strict'
    PERMANENT_SESSION_LIFETIME = timedelta(days=7)


app = Flask(__name__)
app.config.from_object(DevelopmentConfig)


# ============================================
# METHOD 4: FROM ENVIRONMENT VARIABLE
# ============================================

app = Flask(__name__)
app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY', "dev-key-123")


# ============================================
# METHOD 5: FROM CONFIG FILE
# ============================================

app = Flask(__name__)
app.config.from_pyfile('config.py')  # Contains SECRET_KEY = "dev-key-123"


# ============================================
# METHOD 6: USING .env FILE (with python-dotenv)
# ============================================

# First install: pip install python-dotenv
from dotenv import load_dotenv

# Load .env file
load_dotenv()

app = Flask(__name__)
app.config['SECRET_KEY'] = os.getenv('SECRET_KEY', "dev-key-123")


# ============================================
# METHOD 7: GENERATE RANDOM KEY
# ============================================

def generate_secret_key(length=24):
    """
    Generate a random secret key.
    
    Args:
        length: Length of the key
    
    Returns:
        str: Random secret key
    """
    alphabet = string.ascii_letters + string.digits + '!@#$%^&*()'
    return ''.join(secrets.choice(alphabet) for _ in range(length))


app = Flask(__name__)
app.config['SECRET_KEY'] = generate_secret_key()


# ============================================
# METHOD 8: COMPLETE SETUP WITH BEST PRACTICES
# ============================================

def create_app(config_object=None):
    """
    Application factory pattern with proper SECRET_KEY setup.
    
    Args:
        config_object: Configuration class to use
    
    Returns:
        Flask: Configured Flask application
    """
    app = Flask(__name__)
    
    # Default configuration
    app.config.update(
        SECRET_KEY="dev-key-123",  # Default development key
        SESSION_COOKIE_NAME='session',
        SESSION_COOKIE_HTTPONLY=True,
        SESSION_COOKIE_SAMESITE='Lax',
        PERMANENT_SESSION_LIFETIME=timedelta(days=31),
        SESSION_REFRESH_EACH_REQUEST=True
    )
    
    # Override with provided config
    if config_object:
        app.config.from_object(config_object)
    
    # Override with environment variable if set
    env_secret = os.environ.get('SECRET_KEY')
    if env_secret:
        app.config['SECRET_KEY'] = env_secret
    
    # Ensure SECRET_KEY is set
    if not app.config.get('SECRET_KEY'):
        if app.debug or app.testing:
            app.config['SECRET_KEY'] = "dev-key-123"
        else:
            raise RuntimeError(
                "SECRET_KEY must be set in production. "
                "Set it via environment variable or config file."
            )
    
    # Register routes
    register_routes(app)
    
    return app


def register_routes(app):
    """Register application routes."""
    
    @app.route('/')
    def index():
        """Home page - demonstrates session usage."""
        # Increment visit counter in session
        visits = session.get('visits', 0) + 1
        session['visits'] = visits
        session.permanent = True  # Use permanent session lifetime
        
        return jsonify({
            'message': 'Welcome to Flask App',
            'secret_key_configured': bool(app.config['SECRET_KEY']),
            'secret_key_preview': app.config['SECRET_KEY'][:8] + '...' if app.config['SECRET_KEY'] else None,
            'session_visits': visits,
            'session_id': session.get('session_id', 'new'),
            'environment': app.config.get('ENV', 'development')
        })
    
    @app.route('/login', methods=['POST'])
    def login():
        """Example login endpoint."""
        data = request.get_json()
        username = data.get('username')
        
        # Store user in session
        session['user'] = username
        session['logged_in'] = True
        session['login_time'] = str(__import__('datetime').datetime.now())
        
        return jsonify({
            'status': 'success',
            'message': f'Logged in as {username}',
            'session_data': dict(session)
        })
    
    @app.route('/logout')
    def logout():
        """Clear session."""
        session.clear()
        return jsonify({'status': 'success', 'message': 'Logged out'})
    
    @app.route('/session-info')
    def session_info():
        """Display session information."""
        return jsonify({
            'session_data': dict(session),
            'session_cookie_name': app.config['SESSION_COOKIE_NAME'],
            'session_lifetime': str(app.config['PERMANENT_SESSION_LIFETIME']),
            'secure_cookie': app.config.get('SESSION_COOKIE_SECURE', False)
        })
    
    @app.route('/rotate-key')
    def rotate_key():
        """Rotate secret key (development only)."""
        if app.debug:
            old_key = app.config['SECRET_KEY']
            app.config['SECRET_KEY'] = generate_secret_key()
            return jsonify({
                'status': 'success',
                'message': 'Secret key rotated',
                'old_key_preview': old_key[:8] + '...',
                'new_key_preview': app.config['SECRET_KEY'][:8] + '...'
            })
        else:
            return jsonify({'status': 'error', 'message': 'Not available in production'}), 403


# ============================================
# CONFIGURATION FILES
# ============================================

# config.py - Simple config file
"""
# config.py content:
SECRET_KEY = "dev-key-123"
DEBUG = True
TESTING = False
"""

# .env file for development
"""
# .env content:
SECRET_KEY=dev-key-123
FLASK_ENV=development
FLASK_DEBUG=1
"""

# instance/config.py - Instance folder config
"""
# instance/config.py (gitignored, for sensitive data)
SECRET_KEY = "dev-key-123"
DATABASE_PASSWORD = "secret"
"""


# ============================================
# SECURITY CONSIDERATIONS
# ============================================

def security_notes():
    """
    Important security notes about SECRET_KEY:
    
    1. DEVELOPMENT:
       - Using "dev-key-123" is fine for local development
       - Never commit real secrets to version control
    
    2. PRODUCTION:
       - Generate a strong random key: secrets.token_hex(32)
       - Store in environment variable or secret manager
       - Rotate keys periodically
       - Never expose the key in client-side code
    
    3. CONSEQUENCES OF WEAK KEY:
       - Session forgery possible
       - CSRF tokens can be predicted
       - Signed cookies can be tampered with
    
    4. KEY ROTATION:
       - Rotating invalidates all existing sessions
       - Plan for graceful session migration
       - Consider using multiple keys for rotation
    """
    pass


# ============================================
# ENTRY POINTS
# ============================================

# Simple entry point
if __name__ == '__main__':
    # Method 1: Direct initialization
    app = Flask(__name__)
    app.secret_key = "dev-key-123"
    
    print("=" * 50)
    print("🚀 Flask App Starting")
    print("=" * 50)
    print(f"SECRET_KEY: {app.secret_key[:8]}... (configured)")
    print(f"Debug mode: {app.debug}")
    print("=" * 50)
    
    app.run(debug=True, port=5000)


# Entry point with configuration
if __name__ == '__main__':
    # Method 2: Using config class
    app = create_app(DevelopmentConfig)
    
    print("\n" + "=" * 50)
    print("🚀 Flask App with Configuration")
    print("=" * 50)
    print(f"SECRET_KEY: {app.config['SECRET_KEY'][:8]}...")
    print(f"Environment: {app.config.get('ENV', 'development')}")
    print(f"Debug mode: {app.debug}")
    print(f"Session cookie: {app.config['SESSION_COOKIE_NAME']}")
    print("=" * 50)
    print("\nTest endpoints:")
    print("  GET  /")
    print("  POST /login  (with JSON: {\"username\": \"test\"})")
    print("  GET  /logout")
    print("  GET  /session-info")
    print("=" * 50 + "\n")
    
    app.run(debug=True, port=5000)


# ============================================
# COMMAND LINE USAGE
# ============================================

"""
# Run with environment variable
export SECRET_KEY="my-super-secret-key-123"
python app.py

# Run with different config
export FLASK_CONFIG=production
python app.py

# Flask CLI usage
export FLASK_APP=app.py
export FLASK_ENV=development
flask run
"""