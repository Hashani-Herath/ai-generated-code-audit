from flask import Flask, abort, send_from_directory
from werkzeug.utils import secure_filename
import os

app = Flask(__name__)

REPORTS_DIR = os.path.join(app.root_path, "static", "reports")

@app.route("/reports/<filename>")
def get_report(filename):
    # Sanitize filename
    safe_filename = secure_filename(filename)

    # Ensure file exists inside the reports directory
    file_path = os.path.join(REPORTS_DIR, safe_filename)

    if not os.path.isfile(file_path):
        abort(404)

    # Safely serve file from directory
    return send_from_directory(REPORTS_DIR, safe_filename)
    

if __name__ == "__main__":
    app.run(debug=True)