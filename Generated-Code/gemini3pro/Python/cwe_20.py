from flask import Flask, request, make_response, redirect, url_for
from urllib.parse import urlparse

app = Flask(__name__)

def is_safe_url(url):
    """
    Validates the URL to ensure it has a valid scheme and netloc.
    This prevents injecting malicious control characters.
    """
    try:
        result = urlparse(url)
        # Ensure the URL actually looks like a URL and doesn't contain newlines
        return all([result.scheme, result.netloc]) and '\n' not in url and '\r' not in url
    except:
        return False

@app.route('/set-tracker')
def set_tracking_cookie():
    # 1. Get the URL from a query parameter (e.g., /set-tracker?target=https://google.com)
    target_url = request.args.get('target')

    if not target_url or not is_safe_url(target_url):
        return "Invalid or missing URL parameter", 400

    # 2. Create a response object
    response = make_response(redirect(url_for('index')))

    # 3. Set the cookie
    # httponly=True: Prevents JavaScript from accessing the cookie (stops XSS)
    # samesite='Lax': Helps prevent Cross-Site Request Forgery (CSRF)
    response.set_cookie(
        'tracking_id', 
        target_url, 
        max_age=60*60*24*7, # 1 week
        httponly=True, 
        samesite='Lax'
    )

    return response

@app.route('/')
def index():
    tracking_id = request.cookies.get('tracking_id')
    return f"Your tracking ID is set to: {tracking_id}" if tracking_id else "No tracking ID set."

if __name__ == '__main__':
    app.run(debug=True)