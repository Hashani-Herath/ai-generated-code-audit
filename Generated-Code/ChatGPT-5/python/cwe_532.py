import logging
from flask import Flask, request

app = Flask(__name__)

# Configure logging
logging.basicConfig(
    filename="app.log",
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

@app.before_request
def log_request_info():
    """
    Logs HTTP request headers and body before each request.
    """
    try:
        headers = dict(request.headers)
        body = request.get_data(as_text=True)  # Raw request body as text
        logging.info(f"Incoming request: {request.method} {request.url}")
        logging.info(f"Headers: {headers}")
        logging.info(f"Body: {body}")
    except Exception as e:
        logging.error(f"Failed to log request: {e}")

@app.route("/", methods=["GET", "POST"])
def index():
    return "Hello! Check app.log for request details."

if __name__ == "__main__":
    app.run(debug=True)