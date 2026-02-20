from flask import Flask, request, make_response, redirect, url_for, render_template_string
import uuid
import hashlib
from datetime import datetime, timedelta
import re
import urllib.parse

app = Flask(__name__)
app.secret_key = 'your-secret-key-here'  # Change this in production!

# Simple HTML template for the demo
HTML_TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <title>Cookie Tracker Demo</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 600px; margin: 0 auto; }
        .form-group { margin-bottom: 20px; }
        input[type=text] { width: 100%; padding: 8px; margin-top: 5px; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; cursor: pointer; }
        .cookie-info { background: #f0f0f0; padding: 15px; border-radius: 5px; margin-top: 20px; }
        .warning { color: #856404; background: #fff3cd; padding: 10px; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🍪 Tracking ID Cookie Setter</h1>
        
        <div class="warning">
            ⚠️ This is a demo. In production, validate URLs and use secure cookie settings!
        </div>
        
        <form method="POST" action="/set-tracking-cookie">
            <div class="form-group">
                <label for="url">Enter URL to track:</label>
                <input type="text" id="url" name="url" placeholder="https://example.com" required>
            </div>
            
            <div class="form-group">
                <label for="campaign">Campaign (optional):</label>
                <input type="text" id="campaign" name="campaign" placeholder="summer_sale">
            </div>
            
            <button type="submit">Set Tracking Cookie & Redirect</button>
        </form>
        
        {% if cookies %}
        <div class="cookie-info">
            <h3>Current Cookies:</h3>
            <ul>
            {% for name, value in cookies.items() %}
                <li><strong>{{ name }}:</strong> {{ value }}</li>
            {% endfor %}
            </ul>
        </div>
        {% endif %}
        
        <p><a href="/">Refresh</a> | <a href="/clear-cookie">Clear Tracking Cookie</a></p>
    </div>
</body>
</html>
'''


def generate_tracking_id(url, campaign=None):
    """
    Generate a unique tracking ID based on URL and optional campaign.
    
    Args:
        url (str): The URL to track
        campaign (str, optional): Campaign name
    
    Returns:
        str: Unique tracking ID
    """
    # Create a unique identifier
    unique_string = f"{url}_{campaign}_{datetime.utcnow().timestamp()}_{uuid.uuid4()}"
    
    # Hash it for consistency
    tracking_id = hashlib.sha256(unique_string.encode()).hexdigest()[:32]
    
    return tracking_id


def validate_url(url):
    """
    Basic URL validation.
    
    Args:
        url (str): URL to validate
    
    Returns:
        bool: True if valid
    """
    # Basic URL pattern matching
    url_pattern = re.compile(
        r'^https?://'  # http:// or https://
        r'(?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+[A-Z]{2,6}\.?|'  # domain...
        r'localhost|'  # localhost...
        r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})'  # ...or ip
        r'(?::\d+)?'  # optional port
        r'(?:/?|[/?]\S+)$', re.IGNORECASE)
    
    return bool(url_pattern.match(url))


@app.route('/')
def index():
    """Home page showing current cookies."""
    cookies = dict(request.cookies)
    return render_template_string(HTML_TEMPLATE, cookies=cookies)


@app.route('/set-tracking-cookie', methods=['POST'])
def set_tracking_cookie():
    """
    Set tracking_id cookie based on user-provided URL and redirect.
    """
    # Get URL from form
    url = request.form.get('url', '').strip()
    campaign = request.form.get('campaign', '').strip() or None
    
    # Validate URL
    if not url:
        return "Error: No URL provided", 400
    
    if not validate_url(url):
        return "Error: Invalid URL format. Please use http:// or https://", 400
    
    # Parse URL to ensure it's safe
    parsed_url = urllib.parse.urlparse(url)
    
    # Generate tracking ID
    tracking_id = generate_tracking_id(url, campaign)
    
    # Create response with redirect
    response = make_response(redirect(url))
    
    # Set cookie with security options
    response.set_cookie(
        'tracking_id',
        value=tracking_id,
        max_age=timedelta(days=30),  # 30 days
        expires=datetime.utcnow() + timedelta(days=30),
        path='/',
        domain=None,  # Only set for current domain
        secure=False,  # Set to True in production (HTTPS only)
        httponly=True,  # Prevents JavaScript access (XSS protection)
        samesite='Lax'  # CSRF protection
    )
    
    # Also set campaign cookie if provided
    if campaign:
        response.set_cookie(
            'campaign',
            value=campaign,
            max_age=timedelta(days=30),
            httponly=True,
            samesite='Lax'
        )
    
    # Set referrer tracking
    response.set_cookie(
        'tracking_referrer',
        value=parsed_url.netloc,
        max_age=timedelta(days=30),
        httponly=True,
        samesite='Lax'
    )
    
    return response


@app.route('/set-tracking-json', methods=['POST'])
def set_tracking_json():
    """
    API endpoint to set tracking cookie via JSON.
    """
    if not request.is_json:
        return {"error": "Content-Type must be application/json"}, 400
    
    data = request.get_json()
    url = data.get('url', '').strip()
    campaign = data.get('campaign')
    
    if not url:
        return {"error": "URL is required"}, 400
    
    if not validate_url(url):
        return {"error": "Invalid URL format"}, 400
    
    tracking_id = generate_tracking_id(url, campaign)
    
    response = make_response({"success": True, "tracking_id": tracking_id})
    
    response.set_cookie(
        'tracking_id',
        value=tracking_id,
        max_age=timedelta(days=30),
        httponly=True,
        samesite='Lax'
    )
    
    return response


@app.route('/clear-cookie')
def clear_cookie():
    """Clear the tracking cookie."""
    response = make_response(redirect(url_for('index')))
    response.set_cookie('tracking_id', '', expires=0)
    response.set_cookie('campaign', '', expires=0)
    response.set_cookie('tracking_referrer', '', expires=0)
    return response


@app.route('/tracking-info')
def tracking_info():
    """Display tracking information."""
    tracking_id = request.cookies.get('tracking_id')
    campaign = request.cookies.get('campaign')
    referrer = request.cookies.get('tracking_referrer')
    
    if not tracking_id:
        return "No tracking cookie found. <a href='/'>Go back</a>"
    
    return f"""
    <h1>Tracking Information</h1>
    <ul>
        <li><strong>Tracking ID:</strong> {tracking_id}</li>
        <li><strong>Campaign:</strong> {campaign or 'None'}</li>
        <li><strong>Original Referrer:</strong> {referrer or 'None'}</li>
        <li><strong>First Visit:</strong> {request.cookies.get('first_visit', 'Unknown')}</li>
    </ul>
    <p><a href='/'>Back to Home</a></p>
    """


@app.before_request
def track_visits():
    """Track first visit timestamp."""
    if not request.cookies.get('first_visit'):
        # This will be set on the response
        pass  # We'll set it in the actual response


@app.after_request
def add_security_headers(response):
    """Add security headers to responses."""
    response.headers['X-Content-Type-Options'] = 'nosniff'
    response.headers['X-Frame-Options'] = 'SAMEORIGIN'
    return response


# Simple function version (non-Flask)
def create_tracking_cookie_html(url, tracking_id=None):
    """
    Generate HTML with JavaScript to set tracking cookie.
    Useful for static sites or email marketing.
    
    Args:
        url (str): URL to redirect to
        tracking_id (str, optional): Custom tracking ID
    
    Returns:
        str: HTML with cookie-setting JavaScript
    """
    if not tracking_id:
        tracking_id = generate_tracking_id(url)
    
    html = f'''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Tracking Redirect</title>
        <script>
            // Set cookie with JavaScript
            function setCookie(name, value, days) {{
                var expires = "";
                if (days) {{
                    var date = new Date();
                    date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
                    expires = "; expires=" + date.toUTCString();
                }}
                document.cookie = name + "=" + (value || "") + expires + "; path=/; SameSite=Lax";
            }}
            
            // Set tracking cookie and redirect
            setCookie('tracking_id', '{tracking_id}', 30);
            setCookie('tracking_referrer', '{urllib.parse.urlparse(url).netloc}', 30);
            
            // Redirect after setting cookies
            window.location.href = '{url}';
        </script>
        <noscript>
            <meta http-equiv="refresh" content="0;url={url}">
        </noscript>
    </head>
    <body>
        <p>Redirecting to <a href="{url}">{url}</a>...</p>
        <p>If you are not redirected, <a href="{url}">click here</a>.</p>
    </body>
    </html>
    '''
    
    return html


# Command-line interface
def cli_set_tracking_cookie():
    """Command-line version to generate tracking HTML."""
    import sys
    
    print("=== Tracking Cookie Generator ===\n")
    
    url = input("Enter destination URL: ").strip()
    
    if not validate_url(url):
        print("❌ Invalid URL format")
        return
    
    campaign = input("Enter campaign name (optional): ").strip()
    
    tracking_id = generate_tracking_id(url, campaign if campaign else None)
    
    print(f"\n✅ Tracking ID generated: {tracking_id}")
    
    # Generate HTML
    html = create_tracking_cookie_html(url, tracking_id)
    
    # Save to file
    filename = f"tracking_{tracking_id[:8]}.html"
    with open(filename, 'w') as f:
        f.write(html)
    
    print(f"✅ Tracking page saved to: {filename}")
    print(f"📝 Share this file or upload to your server")


if __name__ == '__main__':
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == '--cli':
        cli_set_tracking_cookie()
    else:
        print("Starting Flask server...")
        print("Open http://localhost:5000 in your browser")
        app.run(debug=True, port=5000)