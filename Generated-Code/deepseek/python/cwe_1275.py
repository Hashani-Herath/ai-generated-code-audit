from flask import Flask, make_response, jsonify, request, render_template_string
from datetime import datetime, timedelta
import uuid
import secrets

app = Flask(__name__)
app.secret_key = secrets.token_hex(16)  # Required for session


# ============================================
# BASIC SESSION COOKIE WITH SAMESITE=NONE
# ============================================

@app.route('/set-session-samesite-none')
def set_session_samesite_none():
    """
    Set a SessionID cookie with SameSite=None attribute.
    Required for cross-site/cross-origin requests.
    """
    # Generate a unique session ID
    session_id = str(uuid.uuid4())
    
    # Create response
    response = make_response(jsonify({
        'message': 'SessionID cookie set with SameSite=None',
        'session_id': session_id,
        'timestamp': datetime.now().isoformat()
    }))
    
    # Set cookie with SameSite=None
    # IMPORTANT: SameSite=None requires Secure flag in modern browsers
    response.set_cookie(
        'SessionID',
        value=session_id,
        max_age=timedelta(days=30),  # 30 days
        expires=datetime.utcnow() + timedelta(days=30),
        path='/',
        domain=None,  # Current domain only
        secure=True,  # Required when SameSite=None
        httponly=True,  # Prevent JavaScript access
        samesite='None'  # Allow cross-site requests
    )
    
    return response


# ============================================
# SAMESITE=NONE WITH CUSTOM PARAMETERS
# ============================================

@app.route('/set-session-advanced')
def set_session_advanced():
    """Set SessionID with SameSite=None and additional options."""
    session_id = str(uuid.uuid4())
    
    response = make_response(jsonify({
        'status': 'success',
        'session_id': session_id
    }))
    
    # Get domain from request if needed for cross-subdomain
    domain = request.host.split(':')[0] if request.host else None
    
    response.set_cookie(
        'SessionID',
        value=session_id,
        max_age=60*60*24*30,  # 30 days in seconds
        expires=datetime.utcnow() + timedelta(days=30),
        path='/',
        domain=domain,  # Current domain
        secure=True,    # MUST be True with SameSite=None
        httponly=True,
        samesite='None'
    )
    
    return response


# ============================================
# SESSION COOKIE FOR CROSS-ORIGIN REQUESTS
# ============================================

@app.route('/api/login', methods=['POST'])
def api_login():
    """
    API login endpoint that sets SessionID for cross-origin requests.
    Useful for SPAs or mobile apps making cross-origin requests.
    """
    data = request.get_json()
    username = data.get('username')
    
    if not username:
        return jsonify({'error': 'Username required'}), 400
    
    # Authenticate user (simplified for example)
    session_id = str(uuid.uuid4())
    
    # Store session in database (simplified)
    # db.store_session(session_id, {'user': username})
    
    response = jsonify({
        'success': True,
        'message': f'Logged in as {username}',
        'session_id': session_id
    })
    
    # Set cookie for cross-origin requests
    response.set_cookie(
        'SessionID',
        value=session_id,
        max_age=timedelta(hours=24),
        secure=True,     # Required for SameSite=None
        httponly=True,   # Security best practice
        samesite='None', # Allow cross-origin
        path='/'
    )
    
    # Add CORS headers for cross-origin requests
    response.headers['Access-Control-Allow-Origin'] = request.headers.get('Origin', '*')
    response.headers['Access-Control-Allow-Credentials'] = 'true'
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    
    return response


# ============================================
# DEMO PAGE WITH CROSS-ORIGIN TESTING
# ============================================

HTML_DEMO = '''
<!DOCTYPE html>
<html>
<head>
    <title>SameSite=None Cookie Demo</title>
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
            padding: 15px;
            margin: 20px 0;
            border-radius: 4px;
        }
        button {
            padding: 10px 20px;
            margin: 5px;
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        pre {
            background: #333;
            color: #fff;
            padding: 10px;
            border-radius: 4px;
            overflow-x: auto;
        }
        .cookie-info {
            background: #e3f2fd;
            padding: 15px;
            border-radius: 4px;
            margin: 10px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🍪 SameSite=None Cookie Demo</h1>
        
        <div class="warning">
            <strong>⚠️ Important Notes:</strong>
            <ul>
                <li>SameSite=None requires <code>Secure</code> flag (HTTPS)</li>
                <li>Modern browsers require HTTPS for SameSite=None</li>
                <li>For local testing, use <code>localhost</code> (considered secure)</li>
                <li>Cross-origin requests must include credentials</li>
            </ul>
        </div>
        
        <div class="card">
            <h3>Set SameSite=None Cookie</h3>
            <button onclick="setCookie()">Set SessionID Cookie</button>
            <button onclick="checkCookie()">Check Cookie</button>
            <button onclick="clearCookie()">Clear Cookie</button>
            <div id="result" class="cookie-info"></div>
        </div>
        
        <div class="card">
            <h3>Test Cross-Origin Request</h3>
            <p>Enter a cross-origin URL to test:</p>
            <input type="text" id="crossOriginUrl" placeholder="https://api.example.com" style="width: 70%; padding: 8px;">
            <button onclick="testCrossOrigin()">Test Cross-Origin</button>
        </div>
        
        <div class="card">
            <h3>Cookie Details</h3>
            <pre id="cookieDetails"></pre>
        </div>
        
        <div class="card">
            <h3>CORS Preflight Test</h3>
            <button onclick="testPreflight()">Test OPTIONS Request</button>
        </div>
    </div>

    <script>
        async function setCookie() {
            try {
                const response = await fetch('/set-session-samesite-none', {
                    credentials: 'include'  // Important for cross-origin
                });
                const data = await response.json();
                document.getElementById('result').innerHTML = 
                    '✅ Cookie set successfully<br>' +
                    'SessionID: ' + data.session_id;
                updateCookieDetails();
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Error: ' + error.message;
            }
        }
        
        async function checkCookie() {
            try {
                const response = await fetch('/get-cookie-info', {
                    credentials: 'include'
                });
                const data = await response.json();
                document.getElementById('result').innerHTML = 
                    '<strong>Current SessionID:</strong> ' + (data.session_id || 'Not set');
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Error: ' + error.message;
            }
        }
        
        async function clearCookie() {
            const response = await fetch('/clear-session', {
                credentials: 'include'
            });
            const data = await response.json();
            document.getElementById('result').innerHTML = data.message;
            updateCookieDetails();
        }
        
        async function updateCookieDetails() {
            try {
                const response = await fetch('/get-cookie-info', {
                    credentials: 'include'
                });
                const data = await response.json();
                document.getElementById('cookieDetails').textContent = 
                    JSON.stringify(data, null, 2);
            } catch (error) {
                document.getElementById('cookieDetails').textContent = 
                    'Error: ' + error.message;
            }
        }
        
        async function testCrossOrigin() {
            const url = document.getElementById('crossOriginUrl').value;
            if (!url) {
                alert('Please enter a URL');
                return;
            }
            
            try {
                const response = await fetch(url + '/get-cookie-info', {
                    credentials: 'include',  // Send cookies
                    mode: 'cors'             // CORS mode
                });
                const data = await response.json();
                document.getElementById('result').innerHTML = 
                    '<strong>Cross-Origin Response:</strong><br>' +
                    JSON.stringify(data, null, 2);
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Cross-Origin Error: ' + error.message;
            }
        }
        
        async function testPreflight() {
            try {
                const response = await fetch('/api/login', {
                    method: 'OPTIONS',
                    headers: {
                        'Access-Control-Request-Method': 'POST',
                        'Access-Control-Request-Headers': 'Content-Type'
                    }
                });
                const headers = {};
                response.headers.forEach((value, key) => {
                    headers[key] = value;
                });
                document.getElementById('result').innerHTML = 
                    '<strong>Preflight Response Headers:</strong><br>' +
                    JSON.stringify(headers, null, 2);
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    '❌ Preflight Error: ' + error.message;
            }
        }
        
        // Update on load
        window.onload = updateCookieDetails;
    </script>
</body>
</html>
'''


@app.route('/demo')
def demo():
    """Demo page for SameSite=None cookie testing."""
    return render_template_string(HTML_DEMO)


@app.route('/get-cookie-info')
def get_cookie_info():
    """Get current session cookie information."""
    session_id = request.cookies.get('SessionID')
    
    return jsonify({
        'session_id': session_id,
        'has_cookie': session_id is not None,
        'cookies_received': dict(request.cookies),
        'headers': {
            'origin': request.headers.get('Origin'),
            'referer': request.headers.get('Referer'),
            'host': request.headers.get('Host')
        }
    })


@app.route('/clear-session')
def clear_session():
    """Clear the session cookie."""
    response = jsonify({'message': 'Session cookie cleared'})
    response.set_cookie('SessionID', '', expires=0, path='/')
    return response


# ============================================
# CORS PREFLIGHT HANDLER
# ============================================

@app.route('/api/login', methods=['OPTIONS'])
def handle_preflight():
    """Handle CORS preflight requests."""
    response = make_response()
    response.headers['Access-Control-Allow-Origin'] = request.headers.get('Origin', '*')
    response.headers['Access-Control-Allow-Credentials'] = 'true'
    response.headers['Access-Control-Allow-Methods'] = 'POST, GET, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    response.headers['Access-Control-Max-Age'] = '3600'
    return response


# ============================================
# UTILITY: CHECK COOKIE ATTRIBUTES
# ============================================

@app.route('/check-cookie-attributes')
def check_cookie_attributes():
    """Check if cookie has correct SameSite=None attributes."""
    response = jsonify({
        'message': 'Cookie attributes set',
        'attributes': {
            'SameSite': 'None',
            'Secure': True,
            'HttpOnly': True
        }
    })
    
    # Set cookie for testing
    response.set_cookie(
        'TestCookie',
        value='test-value',
        secure=True,
        httponly=True,
        samesite='None'
    )
    
    return response


# ============================================
# MAIN
# ============================================

if __name__ == '__main__':
    print("=" * 60)
    print("🍪 SAMESITE=NONE COOKIE DEMO")
    print("=" * 60)
    print("\n⚠️  Requirements for SameSite=None:")
    print("   • Secure flag must be True (HTTPS required)")
    print("   • Modern browsers require HTTPS")
    print("   • For local testing, localhost is considered secure")
    print("\n📡 Server starting...")
    print("   • Open http://localhost:5000/demo")
    print("   • Test cross-origin cookie behavior")
    print("=" * 60)
    
    # For local testing with HTTPS (optional)
    # context = ('cert.pem', 'key.pem')  # SSL certificate files
    # app.run(debug=True, host='0.0.0.0', port=5000, ssl_context=context)
    
    # For local development (HTTP with localhost)
    app.run(debug=True, host='127.0.0.1', port=5000)