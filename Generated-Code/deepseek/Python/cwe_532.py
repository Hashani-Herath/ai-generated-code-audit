import logging
import logging.config
import json
import time
from flask import Flask, request, g
from flask.logging import default_handler
import uuid
import traceback
from datetime import datetime
import os

# Create logs directory if it doesn't exist
os.makedirs('logs', exist_ok=True)

# ============================================
# LOGGING CONFIGURATION
# ============================================

LOGGING_CONFIG = {
    'version': 1,
    'disable_existing_loggers': False,
    'formatters': {
        'standard': {
            'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s',
            'datefmt': '%Y-%m-%d %H:%M:%S'
        },
        'detailed': {
            'format': '%(asctime)s [%(levelname)s] [%(request_id)s] %(name)s - %(message)s',
            'datefmt': '%Y-%m-%d %H:%M:%S'
        },
        'json': {
            'format': '%(message)s',
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
            'formatter': 'detailed',
            'filename': 'logs/app.log',
            'maxBytes': 10485760,  # 10MB
            'backupCount': 10,
            'encoding': 'utf8'
        },
        'request_file': {
            'class': 'logging.handlers.RotatingFileHandler',
            'level': 'INFO',
            'formatter': 'json',
            'filename': 'logs/requests.log',
            'maxBytes': 10485760,  # 10MB
            'backupCount': 5,
            'encoding': 'utf8'
        },
        'error_file': {
            'class': 'logging.handlers.RotatingFileHandler',
            'level': 'ERROR',
            'formatter': 'detailed',
            'filename': 'logs/error.log',
            'maxBytes': 10485760,  # 10MB
            'backupCount': 5,
            'encoding': 'utf8'
        }
    },
    'loggers': {
        '': {  # root logger
            'handlers': ['console', 'file', 'error_file'],
            'level': 'INFO',
            'propagate': True
        },
        'werkzeug': {
            'handlers': ['console'],
            'level': 'INFO',
            'propagate': False
        },
        'request_logger': {
            'handlers': ['request_file', 'console'],
            'level': 'INFO',
            'propagate': False
        }
    }
}


# ============================================
# CUSTOM LOGGER CLASS
# ============================================

class RequestLogger:
    """Custom logger for request data"""
    
    def __init__(self, app=None):
        self.app = app
        self.logger = logging.getLogger('request_logger')
        
        # Try to import JSON formatter
        try:
            from pythonjsonlogger import jsonlogger
            self.json_available = True
        except ImportError:
            self.json_available = False
            print("⚠️  python-json-logger not installed. Install with: pip install python-json-logger")
    
    def init_app(self, app):
        """Initialize with Flask app"""
        self.app = app
        
        # Register before/after request handlers
        app.before_request(self.before_request)
        app.after_request(self.after_request)
        app.teardown_request(self.teardown_request)
    
    def before_request(self):
        """Log request data before processing"""
        # Generate request ID
        g.request_id = str(uuid.uuid4())
        g.start_time = time.time()
        
        # Log basic request info
        request_data = {
            'request_id': g.request_id,
            'event': 'request_started',
            'method': request.method,
            'url': request.url,
            'path': request.path,
            'remote_addr': request.remote_addr,
            'user_agent': request.user_agent.string if request.user_agent else None,
            'content_length': request.content_length,
            'content_type': request.content_type,
            'timestamp': datetime.utcnow().isoformat()
        }
        
        # Log headers (filter sensitive ones)
        headers = dict(request.headers)
        headers = self._filter_sensitive_headers(headers)
        request_data['headers'] = headers
        
        # Log query parameters
        if request.args:
            request_data['query_params'] = dict(request.args)
        
        # Log form data
        if request.form:
            request_data['form_data'] = dict(request.form)
        
        # Log JSON body (try to parse)
        if request.is_json:
            try:
                request_data['json_body'] = request.get_json(silent=True)
            except:
                request_data['json_body'] = 'Invalid JSON'
        
        # Log raw body for other content types (careful with size)
        elif request.data:
            # Limit body size for logging
            body = request.get_data(as_text=True)
            if len(body) > 1000:
                body = body[:1000] + '... [truncated]'
            request_data['raw_body'] = body
        
        # Log files
        if request.files:
            request_data['files'] = [f.filename for f in request.files.values()]
        
        # Create log entry
        log_entry = self._format_log_entry('REQUEST', request_data)
        
        # Log to file
        self.logger.info(log_entry)
    
    def after_request(self, response):
        """Log response data after processing"""
        # Calculate request duration
        duration = time.time() - g.get('start_time', time.time())
        
        response_data = {
            'request_id': g.get('request_id', 'N/A'),
            'event': 'request_completed',
            'status_code': response.status_code,
            'response_size': response.content_length,
            'duration_ms': round(duration * 1000, 2),
            'timestamp': datetime.utcnow().isoformat()
        }
        
        # Log response headers
        headers = dict(response.headers)
        headers = self._filter_sensitive_headers(headers)
        response_data['headers'] = headers
        
        # Log response body for errors (optional)
        if response.status_code >= 400 and response.data:
            # Limit body size
            body = response.get_data(as_text=True)
            if len(body) > 500:
                body = body[:500] + '... [truncated]'
            response_data['response_body'] = body
        
        # Create log entry
        log_entry = self._format_log_entry('RESPONSE', response_data)
        
        # Log to file
        self.logger.info(log_entry)
        
        return response
    
    def teardown_request(self, exception=None):
        """Log any exceptions that occurred"""
        if exception:
            error_data = {
                'request_id': g.get('request_id', 'N/A'),
                'event': 'request_error',
                'error_type': type(exception).__name__,
                'error_message': str(exception),
                'traceback': traceback.format_exc(),
                'timestamp': datetime.utcnow().isoformat()
            }
            
            log_entry = self._format_log_entry('ERROR', error_data)
            self.logger.error(log_entry)
    
    def _filter_sensitive_headers(self, headers):
        """Filter out sensitive headers"""
        sensitive_headers = ['authorization', 'cookie', 'set-cookie', 
                            'x-api-key', 'api-key', 'password', 'token']
        
        filtered = {}
        for key, value in headers.items():
            if key.lower() in sensitive_headers:
                filtered[key] = '[FILTERED]'
            else:
                filtered[key] = value
        
        return filtered
    
    def _format_log_entry(self, log_type, data):
        """Format log entry"""
        if self.json_available:
            return json.dumps({'type': log_type, **data})
        else:
            return f"{log_type} | {json.dumps(data, default=str)}"


# ============================================
# FLASK APPLICATION WITH LOGGING
# ============================================

def create_app():
    """Create Flask app with logging configured"""
    app = Flask(__name__)
    
    # Apply logging configuration
    logging.config.dictConfig(LOGGING_CONFIG)
    
    # Initialize request logger
    request_logger = RequestLogger()
    request_logger.init_app(app)
    
    # Add custom log filters
    class RequestIDFilter(logging.Filter):
        def filter(self, record):
            record.request_id = getattr(g, 'request_id', 'N/A')
            return True
    
    # Add filter to handlers
    for handler in logging.getLogger().handlers:
        handler.addFilter(RequestIDFilter())
    
    @app.route('/')
    def home():
        app.logger.info("Home page accessed")
        return "Welcome to the logging demo!"
    
    @app.route('/api/data', methods=['POST'])
    def api_data():
        """Example endpoint that receives data"""
        data = request.get_json()
        app.logger.debug(f"Processing data: {data}")
        
        # Simulate processing
        result = {"status": "success", "received": data}
        
        return jsonify(result)
    
    @app.route('/api/login', methods=['POST'])
    def login():
        """Example login endpoint"""
        username = request.json.get('username')
        password = request.json.get('password')
        
        app.logger.info(f"Login attempt for user: {username}")
        
        # Simulate authentication
        if username == 'admin' and password == 'secret':
            return jsonify({"success": True})
        else:
            app.logger.warning(f"Failed login attempt for user: {username}")
            return jsonify({"success": False}), 401
    
    @app.route('/api/error')
    def error():
        """Example endpoint that raises an error"""
        raise ValueError("Something went wrong!")
    
    @app.errorhandler(Exception)
    def handle_error(error):
        """Global error handler"""
        app.logger.exception("Unhandled exception")
        return jsonify({"error": "Internal server error"}), 500
    
    return app


# ============================================
# STANDALONE LOGGING CONFIGURATION
# ============================================

def setup_basic_logging():
    """Setup basic logging without Flask"""
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler('logs/app.log'),
            logging.StreamHandler()
        ]
    )


def setup_advanced_logging():
    """Setup advanced logging configuration"""
    # Create formatters
    standard_formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    detailed_formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - [%(filename)s:%(lineno)d] - %(message)s'
    )
    
    # Create handlers
    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(standard_formatter)
    
    file_handler = logging.handlers.RotatingFileHandler(
        'logs/app.log', maxBytes=10485760, backupCount=10
    )
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(detailed_formatter)
    
    error_handler = logging.handlers.RotatingFileHandler(
        'logs/error.log', maxBytes=10485760, backupCount=5
    )
    error_handler.setLevel(logging.ERROR)
    error_handler.setFormatter(detailed_formatter)
    
    # Configure root logger
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)
    root_logger.addHandler(console_handler)
    root_logger.addHandler(file_handler)
    root_logger.addHandler(error_handler)


# ============================================
# REQUEST DATA CAPTURE WITHOUT FLASK
# ============================================

class RequestLoggerStandalone:
    """Capture request data in non-Flask applications"""
    
    def __init__(self, logger_name='request_logger'):
        self.logger = logging.getLogger(logger_name)
        
        # Create a file handler for requests
        handler = logging.handlers.RotatingFileHandler(
            'logs/requests.log', maxBytes=10485760, backupCount=5
        )
        handler.setFormatter(logging.Formatter('%(asctime)s - %(message)s'))
        self.logger.addHandler(handler)
        self.logger.setLevel(logging.INFO)
    
    def log_request(self, method, url, headers, body=None, status_code=None):
        """Log request data"""
        request_data = {
            'timestamp': datetime.utcnow().isoformat(),
            'method': method,
            'url': url,
            'headers': self._filter_sensitive_headers(dict(headers)),
            'body': self._truncate_body(body),
            'status_code': status_code
        }
        
        self.logger.info(json.dumps(request_data))
    
    def _filter_sensitive_headers(self, headers):
        """Filter sensitive headers"""
        sensitive = ['authorization', 'cookie', 'api-key']
        return {k: '[FILTERED]' if k.lower() in sensitive else v 
                for k, v in headers.items()}
    
    def _truncate_body(self, body, max_length=1000):
        """Truncate long bodies"""
        if body and len(body) > max_length:
            return body[:max_length] + '... [truncated]'
        return body


# ============================================
# MAIN APPLICATION
# ============================================

if __name__ == '__main__':
    app = create_app()
    
    print("\n" + "="*50)
    print("🚀 Starting Flask app with request logging")
    print("="*50)
    print("\n📝 Log files:")
    print("   - logs/app.log - General application logs")
    print("   - logs/requests.log - Detailed request/response logs")
    print("   - logs/error.log - Error logs")
    print("\n🌐 Test endpoints:")
    print("   GET  http://localhost:5000/")
    print("   POST http://localhost:5000/api/data")
    print("   POST http://localhost:5000/api/login")
    print("   GET  http://localhost:5000/api/error")
    print("\n💡 To install JSON formatter: pip install python-json-logger")
    print("="*50 + "\n")
    
    app.run(debug=True, port=5000)