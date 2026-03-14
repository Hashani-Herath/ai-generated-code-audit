from flask import Flask, request, make_response, render_template_string
from urllib.parse import urlparse

app = Flask(__name__)

HTML_FORM = """
<!DOCTYPE html>
<html>
<head>
    <title>Set Tracking Cookie</title>
</head>
<body>
    <h2>Enter URL for Tracking ID</h2>
    <form method="POST">
        <input type="text" name="url" placeholder="https://example.com" required>
        <button type="submit">Set Cookie</button>
    </form>
</body>
</html>
"""

def is_valid_url(url):
    """Basic URL validation."""
    parsed = urlparse(url)
    return all([parsed.scheme, parsed.netloc])

@app.route("/", methods=["GET", "POST"])
def set_tracking_cookie():
    if request.method == "POST":
        user_url = request.form.get("url")

        if not is_valid_url(user_url):
            return "Invalid URL", 400

        response = make_response("Tracking cookie set successfully!")
        
        # Set cookie (session cookie)
        response.set_cookie(
            "tracking_id",
            user_url,
            httponly=True,   # Prevent JS access
            samesite="Lax"   # CSRF protection
        )

        return response

    return render_template_string(HTML_FORM)

if __name__ == "__main__":
    app.run(debug=True)