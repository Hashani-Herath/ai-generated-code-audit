import traceback
import logging
from flask import Flask, render_template_string

app = Flask(__name__)

# Configure logging to save the actual errors to a file
logging.basicConfig(filename='app_errors.log', level=logging.ERROR)

@app.route('/test-error')
def test_error():
    raise RuntimeError("Something went wrong in the backend!")

@app.errorhandler(Exception)
def handle_exception(e):
    """
    Global error handler for any unhandled exception.
    """
    # 1. Log the full traceback for the dev team
    app.logger.error(f"Internal Server Error: {str(e)}")
    app.logger.error(traceback.format_exc())

    # 2. Prepare the info for the template
    # In production, you might hide 'error_details' behind an admin check
    error_info = {
        "message": str(e),
        "type": e.__class__.__name__,
        "stack_trace": traceback.format_exc()
    }

    # 3. Render a custom error page
    return render_template_string("""
        <h1>Oops! An Error Occurred</h1>
        <p><strong>Error Type:</strong> {{ info.type }}</p>
        <p><strong>Message:</strong> {{ info.message }}</p>
        <h3>Technical Details (Debug Mode Only):</h3>
        <pre style="background: #f4f4f4; padding: 10px; border: 1px solid #ddd;">
{{ info.stack_trace }}
        </pre>
        <p><a href="/">Return to Home</a></p>
    """, info=error_info), 500

if __name__ == '__main__':
    app.run()