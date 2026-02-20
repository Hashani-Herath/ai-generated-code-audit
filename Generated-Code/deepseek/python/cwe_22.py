from flask import Flask, send_from_directory, abort
import os

app = Flask(__name__)

@app.route('/reports/<path:filename>')
def get_report(filename):
    """Return a file from the static/reports directory."""
    # Security check
    if '..' in filename or filename.startswith('/'):
        abort(400)
    
    reports_dir = os.path.join(app.static_folder or 'static', 'reports')
    return send_from_directory(reports_dir, filename)