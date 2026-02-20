from flask import Flask, make_response, jsonify
from datetime import datetime, timedelta
import uuid

app = Flask(__name__)

# ============================================
# BASIC SESSION COOKIE - NO SECURITY FLAGS
# ============================================

def set_session_cookie_basic(response, session_data, cookie_name='session_id'):
    """
    Set a basic session cookie with NO security flags.
    
    Args:
        response: Flask response object
        session_data: Data to store in cookie (will be converted to string)
        cookie_name: Name of the cookie
    
    Returns:
        Response object with cookie set
    """
    # Convert session data to string if needed
    if not isinstance(session_data, str):
        session_data = str(session_data)
    
    # Set cookie with NO security flags
    response.set_cookie(
        cookie_name,
        value=session_data
        # NO max_age - session cookie (expires when browser closes)
        # NO secure flag - can be sent over HTTP
        # NO httponly flag - accessible to JavaScript
        # NO samesite flag - no restriction
        # NO domain - defaults to current domain only
        # NO path - defaults to '/'
    )
    
    return response


# ============================================
# SESSION COOKIE WITH PERSISTENCE
# ============================================

def set_persistent_session_cookie(response, session_data, days=30):
    """
    Set a persistent session cookie with expiration.
    Still NO security flags.
    """
    response.set_cookie(
        'session_id',
        value=session_data,
        max_age=timedelta(days=days)  # Cookie persists for specified days
        # Still no security flags
    )
    return response


# ============================================
# SESSION COOKIE WITH CUSTOM PATH
# ============================================

def set_session_cookie_path(response, session_data, path='/app'):
    """
    Set session cookie limited to specific path.
    Still NO security flags.
    """
    response.set_cookie(
        'session_id',
        value=session_data,
        path=path  # Only sent for requests under /app
        # Still no security flags
    )
    return response


# ============================================
# COMPLETE EXAMPLE WITH ROUTES
# ============================================

@app.route('/set-session-basic')
def set_basic_session():
    """Set a basic session cookie with NO security flags."""
    # Generate a simple session ID
    session_id = str(uuid.uuid4())
    
    # Create response
    response = make_response(jsonify({
        'message': 'Session cookie set (no security flags)',
        'session_id': session_id,
        'timestamp': datetime.now().isoformat()
    }))
    
    # Set cookie with NO security flags
    response.set_cookie(
        'session_id',
        value=session_id
        # NO secure=True
        # NO httponly=True
        # NO samesite
        # NO max_age (session cookie)
    )
    
    return response


@app.route('/set-session-persistent')
def set_persistent_session():
    """Set a persistent session cookie (30 days) with NO security flags."""
    session_id = str(uuid.uuid4())
    
    response = make_response(jsonify({
        'message': 'Persistent session cookie set (30 days, no security)',
        'session_id': session_id
    }))
    
    response.set_cookie(
        'session_id',
        value=session_id,
        max_age=60*60*24*30  # 30 days in seconds
        # NO security flags
    )
    
    return response


@app.route('/set-multiple-cookies')
def set_multiple_cookies():
    """Set multiple cookies with NO security flags."""
    response = make_response(jsonify({
        'message': 'Multiple cookies set with no security'
    }))
    
    # Session cookie (no expiration)
    response.set_cookie('session_id', str(uuid.uuid4()))
    
    # User preference cookie (persistent)
    response.set_cookie('theme', 'dark', max_age=60*60*24*365)  # 1 year
    
    # Tracking cookie
    response.set_cookie('visitor_id', str(uuid.uuid4()), max_age=60*60*24*30)
    
    # No security flags on any cookie
    
    return response


@app.route('/get-cookies')
def get_cookies():
    """Retrieve all cookies sent by client."""
    cookies = dict(request.cookies)
    return jsonify({
        'cookies_received': cookies,
        'count': len(cookies)
    })


@app.route('/clear-session')
def clear_session():
    """Clear the session cookie."""
    response = make_response(jsonify({'message': 'Session cookie cleared'}))
    response.set_cookie('session_id', '', expires=0)
    return response


# ============================================
# NON-FLASK VERSION (GENERIC)
# ============================================

def create_cookie_header(cookie_name, cookie_value, **kwargs):
    """
    Create a raw Set-Cookie header string without security flags.
    Generic version - not tied to Flask.
    
    Args:
        cookie_name: Name of the cookie
        cookie_value: Value of the cookie
        **kwargs: Additional cookie parameters
    
    Returns:
        str: Set-Cookie header value
    """
    # Build the cookie string
    cookie_parts = [f"{cookie_name}={cookie_value}"]
    
    # Add optional parameters (still no security flags)
    if 'expires' in kwargs:
        cookie_parts.append(f"Expires={kwargs['expires']}")
    if 'max_age' in kwargs:
        cookie_parts.append(f"Max-Age={kwargs['max_age']}")
    if 'path' in kwargs:
        cookie_parts.append(f"Path={kwargs['path']}")
    if 'domain' in kwargs:
        cookie_parts.append(f"Domain={kwargs['domain']}")
    
    # Explicitly NOT adding:
    # - Secure
    # - HttpOnly
    # - SameSite
    
    return '; '.join(cookie_parts)


# ============================================
# EXAMPLE WITH RAW HTTP RESPONSE
# ============================================

def generate_http_response_with_cookie():
    """
    Generate a raw HTTP response with a session cookie.
    No security flags included.
    """
    session_id = str(uuid.uuid4())
    
    response = f"""HTTP/1.1 200 OK
Content-Type: text/html
Set-Cookie: session_id={session_id}
Set-Cookie: user_pref=dark_mode; Max-Age=2592000

<html>
<body>
    <h1>Cookie Set (No Security Flags)</h1>
    <p>Session ID: {session_id}</p>
</body>
</html>"""
    
    return response


# ============================================
# DEMO ROUTE - SHOWS ALL COOKIE VARIANTS
# ============================================

@app.route('/demo')
def demo_page():
    """Demo page showing different cookie setting methods."""
    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Cookie Demo - No Security Flags</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .cookie-box { 
                background: #f0f0f0; 
                padding: 20px; 
                margin: 20px 0;
                border-left: 4px solid #ff6b6b;
            }
            button { 
                padding: 10px 20px; 
                margin: 5px;
                background: #4CAF50; 
                color: white; 
                border: none; 
                cursor: pointer; 
            }
            .warning {
                background: #fff3cd;
                border-left-color: #856404;
                padding: 15px;
                margin: 20px 0;
            }
        </style>
    </head>
    <body>
        <h1>🍪 Session Cookies - NO SECURITY FLAGS</h1>
        
        <div class="warning">
            <strong>⚠️ WARNING:</strong> These cookies have NO security flags:
            <ul>
                <li>❌ Not Secure - Can be sent over HTTP</li>
                <li>❌ Not HttpOnly - Accessible to JavaScript</li>
                <li>❌ No SameSite - Vulnerable to CSRF</li>
            </ul>
        </div>
        
        <div class="cookie-box">
            <h3>Set Cookies (No Security):</h3>
            <button onclick="fetch('/set-session-basic')">Basic Session Cookie</button>
            <button onclick="fetch('/set-session-persistent')">Persistent (30d) Cookie</button>
            <button onclick="fetch('/set-multiple-cookies')">Multiple Cookies</button>
            <button onclick="location.reload()">Refresh Page</button>
        </div>
        
        <div class="cookie-box">
            <h3>JavaScript Access Test:</h3>
            <p>Current cookies from JavaScript:</p>
            <pre id="js-cookies"></pre>
            <button onclick="showCookies()">Show Cookies</button>
            <script>
                function showCookies() {
                    document.getElementById('js-cookies').textContent = 
                        document.cookie || 'No cookies (or HttpOnly)';
                }
            </script>
        </div>
        
        <div class="cookie-box">
            <h3>Server-side Cookies:</h3>
            <button onclick="fetch('/get-cookies').then(r=>r.json()).then(console.log)">
                Check Server Cookies
            </button>
        </div>
        
        <div class="cookie-box">
            <h3>Clear Cookies:</h3>
            <button onclick="fetch('/clear-session')">Clear Session</button>
        </div>
    </body>
    </html>
    """
    return html


# ============================================
# MAIN
# ============================================

if __name__ == '__main__':
    print("=" * 60)
    print("🍪 SESSION COOKIE DEMO - NO SECURITY FLAGS")
    print("=" * 60)
    print("\n⚠️  This demo sets cookies WITHOUT security flags:")
    print("   • ❌ No Secure flag (can be sent over HTTP)")
    print("   • ❌ No HttpOnly flag (accessible to JavaScript)")
    print("   • ❌ No SameSite flag (vulnerable to CSRF)")
    print("\n📡 Server starting at http://localhost:5000")
    print("   Open /demo to see the interactive demo")
    print("=" * 60)
    
    app.run(debug=True, port=5000)