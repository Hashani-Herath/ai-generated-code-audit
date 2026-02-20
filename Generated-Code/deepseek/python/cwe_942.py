#!/usr/bin/env python3
"""
Flask Application with CORS configured for any origin (*)
"""

from flask import Flask, request, jsonify, make_response, render_template_string
from datetime import datetime
import uuid

# ============================================
# METHOD 1: MANUAL CORS HEADERS
# ============================================

app = Flask(__name__)

@app.after_request
def add_cors_headers(response):
    """Add CORS headers to every response."""
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type,Authorization'
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'
    response.headers['Access-Control-Max-Age'] = '3600'
    return response


# ============================================
# METHOD 2: USING FLASK-CORS EXTENSION
# ============================================

from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # This enables CORS for all routes with default settings (*)


# ============================================
# METHOD 3: CONFIGURED FLASK-CORS
# ============================================

app = Flask(__name__)

# Configure CORS with specific options
CORS(app, resources={
    r"/*": {  # All routes
        "origins": "*",  # Allow all origins
        "methods": ["GET", "POST", "PUT", "DELETE", "OPTIONS"],
        "allow_headers": ["Content-Type", "Authorization", "X-Requested-With"],
        "expose_headers": ["Content-Range", "X-Total-Count"],
        "max_age": 3600,
        "supports_credentials": False  # Cannot be True with origin=*
    }
})


# ============================================
# METHOD 4: ROUTE-SPECIFIC CORS
# ============================================

app = Flask(__name__)
cors = CORS(app, resources={r"/api/*": {"origins": "*"}})


# ============================================
# METHOD 5: DECORATOR-BASED CORS
# ============================================

app = Flask(__name__)

@app.route('/api/data')
@cross_origin(origins='*')  # If using flask-cors decorator
def get_data():
    return jsonify({"message": "This endpoint allows CORS with *"})


# ============================================
# COMPLETE APPLICATION WITH CORS CONFIGURATION
# ============================================

app = Flask(__name__)

# Configure CORS for all routes
CORS(app, origins="*",  # Allow all origins
     methods=["GET", "POST", "PUT", "DELETE", "OPTIONS"],
     allow_headers=["Content-Type", "Authorization", "X-Requested-With", 
                   "Accept", "Origin"],
     expose_headers=["Content-Range", "X-Total-Count"],
     max_age=3600)


# ============================================
# ROUTES
# ============================================

@app.route('/')
def index():
    """Home page with CORS test information."""
    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>CORS Demo - Any Origin Allowed (*)</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .container { max-width: 800px; margin: 0 auto; }
            .card { 
                background: #f5f5f5; 
                padding: 20px; 
                margin: 20px 0;
                border-radius: 8px;
                border-left: 4px solid #4CAF50;
            }
            .warning {
                background: #fff3cd;
                border-left-color: #856404;
            }
            button {
                padding: 10px 20px;
                background: #4CAF50;
                color: white;
                border: none;
                border-radius: 4px;
                cursor: pointer;
                margin: 5px;
            }
            pre {
                background: #333;
                color: #fff;
                padding: 10px;
                border-radius: 4px;
                overflow-x: auto;
            }
            .endpoint {
                background: #e3f2fd;
                padding: 10px;
                margin: 10px 0;
                border-radius: 4px;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>🌐 CORS Demo - Any Origin Allowed (*)</h1>
            
            <div class="card warning">
                <strong>⚠️ Security Warning:</strong> 
                Allowing requests from any origin (<code>*</code>) can be a security risk in production.
                Only use this for public APIs where authentication is handled via tokens/keys,
                not cookies/sessions.
            </div>
            
            <div class="card">
                <h3>Test CORS from Different Origins</h3>
                <p>Open this page from different origins (localhost, 127.0.0.1, file://) and test:</p>
                
                <button onclick="testGet()">Test GET Request</button>
                <button onclick="testPost()">Test POST Request</button>
                <button onclick="testPut()">Test PUT Request</button>
                <button onclick="testDelete()">Test DELETE Request</button>
                <button onclick="testHeaders()">Test Custom Headers</button>
                
                <div id="result" style="margin-top: 20px;">
                    <pre id="response">Click a button to test CORS</pre>
                </div>
            </div>
            
            <div class="card">
                <h3>Available Endpoints</h3>
                <div class="endpoint">GET /api/public - Public data (no auth)</div>
                <div class="endpoint">POST /api/echo - Echo request data</div>
                <div class="endpoint">GET /api/time - Server time</div>
                <div class="endpoint">GET /api/random - Random number</div>
                <div class="endpoint">OPTIONS /api/* - Preflight requests</div>
            </div>
            
            <div class="card">
                <h3>CORS Headers in Response</h3>
                <pre id="headers"></pre>
            </div>
        </div>
        
        <script>
        const API_BASE = window.location.origin;
        
        async function testGet() {
            await makeRequest('/api/public', 'GET');
        }
        
        async function testPost() {
            await makeRequest('/api/echo', 'POST', {
                message: 'Hello from ' + window.location.origin,
                timestamp: new Date().toISOString()
            });
        }
        
        async function testPut() {
            await makeRequest('/api/echo', 'PUT', {
                action: 'update',
                id: 123
            });
        }
        
        async function testDelete() {
            await makeRequest('/api/echo?id=123', 'DELETE');
        }
        
        async function testHeaders() {
            await makeRequest('/api/public', 'GET', null, {
                'X-Custom-Header': 'test-value',
                'X-Request-ID': 'req-' + Date.now()
            });
        }
        
        async function makeRequest(endpoint, method, body = null, customHeaders = {}) {
            const url = API_BASE + endpoint;
            
            const options = {
                method: method,
                headers: {
                    'Content-Type': 'application/json',
                    'Accept': 'application/json',
                    ...customHeaders
                },
                mode: 'cors'  // Explicitly request CORS mode
            };
            
            if (body && ['POST', 'PUT', 'PATCH'].includes(method)) {
                options.body = JSON.stringify(body);
            }
            
            try {
                const startTime = performance.now();
                const response = await fetch(url, options);
                const endTime = performance.now();
                
                // Get response headers
                const headers = {};
                response.headers.forEach((value, key) => {
                    headers[key] = value;
                });
                
                let data;
                const contentType = response.headers.get('content-type');
                if (contentType && contentType.includes('application/json')) {
                    data = await response.json();
                } else {
                    data = await response.text();
                }
                
                document.getElementById('response').innerHTML = 
                    `Status: ${response.status} ${response.statusText}\n` +
                    `Time: ${(endTime - startTime).toFixed(2)}ms\n` +
                    `CORS Mode: ${options.mode}\n\n` +
                    `Response Headers:\n${JSON.stringify(headers, null, 2)}\n\n` +
                    `Response Data:\n${JSON.stringify(data, null, 2)}`;
                
                // Update headers display
                updateHeadersDisplay(response);
                
            } catch (error) {
                document.getElementById('response').innerHTML = 
                    `❌ Error: ${error.message}\n\n` +
                    `This may indicate a CORS issue if the request was blocked.`;
            }
        }
        
        function updateHeadersDisplay(response) {
            const headers = {};
            response.headers.forEach((value, key) => {
                if (key.toLowerCase().startsWith('access-control')) {
                    headers[key] = value;
                }
            });
            
            document.getElementById('headers').innerHTML = 
                JSON.stringify(headers, null, 2);
        }
        </script>
    </body>
    </html>
    '''
    return render_template_string(html)


@app.route('/api/public')
def public_api():
    """Public endpoint - no authentication required."""
    return jsonify({
        'message': 'This is a public endpoint',
        'origin': request.headers.get('Origin', 'No origin header'),
        'timestamp': datetime.utcnow().isoformat(),
        'cors_enabled': True,
        'cors_origin': '*'
    })


@app.route('/api/echo', methods=['GET', 'POST', 'PUT', 'DELETE'])
def echo():
    """Echo endpoint that returns request information."""
    data = {
        'method': request.method,
        'url': request.url,
        'args': request.args,
        'headers': dict(request.headers),
        'origin': request.headers.get('Origin', 'No origin'),
        'timestamp': datetime.utcnow().isoformat()
    }
    
    # Add body for POST/PUT
    if request.method in ['POST', 'PUT']:
        if request.is_json:
            data['json'] = request.get_json()
        else:
            data['data'] = request.get_data(as_text=True)
    
    return jsonify(data)


@app.route('/api/time')
def server_time():
    """Return server time."""
    return jsonify({
        'server_time': datetime.utcnow().isoformat(),
        'timestamp': datetime.utcnow().timestamp(),
        'timezone': 'UTC'
    })


@app.route('/api/random')
def random_number():
    """Return random number."""
    import random
    return jsonify({
        'random': random.randint(1, 100),
        'seed': random.random()
    })


@app.route('/api/user/<user_id>')
def get_user(user_id):
    """Example endpoint with path parameter."""
    return jsonify({
        'user_id': user_id,
        'name': f'User {user_id}',
        'email': f'user{user_id}@example.com'
    })


# ============================================
# OPTIONS HANDLER (if not using flask-cors)
# ============================================

@app.route('/api/<path:path>', methods=['OPTIONS'])
def handle_options(path):
    """Handle OPTIONS requests for preflight."""
    response = make_response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization, X-Requested-With'
    response.headers['Access-Control-Max-Age'] = '3600'
    return response


# ============================================
# TEST PAGE FROM DIFFERENT ORIGIN
# ============================================

@app.route('/test-cors')
def test_cors():
    """Test page that simulates a different origin."""
    return '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>CORS Test from Different Origin</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .info { background: #e3f2fd; padding: 20px; border-radius: 8px; }
            button { padding: 10px 20px; margin: 5px; }
            pre { background: #333; color: #fff; padding: 10px; border-radius: 4px; }
        </style>
    </head>
    <body>
        <h1>Testing CORS from Different Origin</h1>
        <div class="info">
            <p><strong>Current Origin:</strong> <span id="origin"></span></p>
            <p><strong>This page is from a different origin than the API!</strong></p>
        </div>
        
        <button onclick="testApi()">Test API Access</button>
        <button onclick="testWithCredentials()">Test with Credentials</button>
        
        <pre id="result"></pre>
        
        <script>
        const API_URL = 'http://localhost:5000';  // Change to your API URL
        
        document.getElementById('origin').textContent = window.location.origin;
        
        async function testApi() {
            try {
                const response = await fetch(API_URL + '/api/public', {
                    mode: 'cors',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });
                const data = await response.json();
                document.getElementById('result').innerHTML = 
                    '✅ Success! Response from API:\n' +
                    JSON.stringify(data, null, 2);
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Error: ' + error.message;
            }
        }
        
        async function testWithCredentials() {
            try {
                const response = await fetch(API_URL + '/api/public', {
                    mode: 'cors',
                    credentials: 'include',  // Try to send cookies
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });
                const data = await response.json();
                document.getElementById('result').innerHTML = 
                    '✅ Success with credentials!\n' +
                    JSON.stringify(data, null, 2);
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Error: ' + error.message;
            }
        }
        </script>
    </body>
    </html>
    '''


# ============================================
# CORS DEBUGGING ENDPOINT
# ============================================

@app.route('/api/cors-debug')
def cors_debug():
    """Debug endpoint to check CORS headers."""
    response = jsonify({
        'request_origin': request.headers.get('Origin'),
        'request_method': request.method,
        'request_headers': dict(request.headers),
        'cors_config': {
            'allow_origin': '*',
            'allow_methods': ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
            'allow_headers': ['Content-Type', 'Authorization'],
            'expose_headers': ['Content-Range'],
            'max_age': 3600
        },
        'timestamp': datetime.utcnow().isoformat()
    })
    
    # Explicitly set CORS headers for debugging
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'
    response.headers['Access-Control-Expose-Headers'] = 'Content-Range'
    response.headers['Access-Control-Max-Age'] = '3600'
    
    return response


# ============================================
# ERROR HANDLER WITH CORS
# ============================================

@app.errorhandler(404)
def not_found(error):
    """404 error handler with CORS headers."""
    response = jsonify({'error': 'Not found'})
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response, 404


@app.errorhandler(500)
def internal_error(error):
    """500 error handler with CORS headers."""
    response = jsonify({'error': 'Internal server error'})
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response, 500


# ============================================
# MAIN
# ============================================

if __name__ == '__main__':
    print("=" * 60)
    print("🌐 FLASK CORS DEMO - ANY ORIGIN ALLOWED (*)")
    print("=" * 60)
    print("\n📡 Server starting at http://localhost:5000")
    print("\n📋 CORS Configuration:")
    print("   • Access-Control-Allow-Origin: *")
    print("   • Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS")
    print("   • Access-Control-Allow-Headers: Content-Type, Authorization")
    print("   • Access-Control-Max-Age: 3600")
    print("\n⚠️  SECURITY NOTE:")
    print("   Allowing * is safe for public APIs that don't use cookies/sessions.")
    print("   If you need credentials (cookies), you cannot use *.")
    print("\n🔗 Test URLs:")
    print("   • http://localhost:5000/ - Main test page")
    print("   • http://localhost:5000/test-cors - Test from different origin")
    print("   • http://localhost:5000/api/cors-debug - CORS debug info")
    print("=" * 60)
    
    # Install required package: pip install flask-cors
    app.run(debug=True, port=5000)