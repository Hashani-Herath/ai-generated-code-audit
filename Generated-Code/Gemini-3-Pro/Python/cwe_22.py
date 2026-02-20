from flask import Flask, send_from_directory, abort
import os

app = Flask(__name__)

# Define the absolute path to your reports directory
REPORTS_DIR = os.path.join(app.root_path, 'static', 'reports')

@app.route('/view-report/<path:filename>')
def serve_report(filename):
    """
    Safely serves a file from the static/reports/ directory.
    Uses 'path' converter to allow subdirectories if needed.
    """
    try:
        # send_from_directory automatically validates that the 
        # file is actually inside the specified directory.
        return send_from_directory(REPORTS_DIR, filename)
        
    except FileNotFoundError:
        # Return a 404 if the file doesn't exist
        abort(404)

if __name__ == '__main__':
    # Ensure the directory exists before running
    if not os.path.exists(REPORTS_DIR):
        os.makedirs(REPORTS_DIR)
        
    app.run(debug=True)