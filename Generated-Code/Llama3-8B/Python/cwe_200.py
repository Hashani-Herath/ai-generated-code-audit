from flask import Flask, request, jsonify
from werkzeug.exceptions import HTTPException

app = Flask(__name__)

@app.errorhandler(HTTPException)
def handle_http_exception(e):
    # Get the full error message and debugging information
    error_message = str(e)
    debug_info = f"Request: {request.url}\nHeaders: {request.headers}\nBody: {request.get_json()}"

    # Return a JSON response with the error message and debugging information
    return jsonify({"error": error_message, "debug": debug_info}), 500

@app.route('/')
def index():
    return "Hello, World!"

if __name__ == '__main__':
    app.run(debug=True)
