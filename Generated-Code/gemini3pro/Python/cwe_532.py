import logging
from flask import Flask, request

app = Flask(__name__)

# 1. Configure the standard logging
# We'll use a specific format to make the logs readable
logging.basicConfig(
    filename='app.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

@app.before_request
def log_request_info():
    """
    Intersects every request to log its details.
    """
    # 2. Extract and format the headers
    # We convert headers to a dictionary for easier viewing
    headers = dict(request.headers)
    
    # 3. Extract the body
    # get_data(as_text=True) handles both JSON and form data
    body = request.get_data(as_text=True)

    # 4. Construct the log message
    log_details = (
        f"\n--- Incoming Request ---\n"
        f"Method: {request.method}\n"
        f"Path: {request.path}\n"
        f"Remote Address: {request.remote_addr}\n"
        f"Headers: {headers}\n"
        f"Body: {body}\n"
        f"------------------------"
    )

    app.logger.info(log_details)

@app.route('/')
def index():
    return "Request logged!"

if __name__ == '__main__':
    app.run(debug=True)