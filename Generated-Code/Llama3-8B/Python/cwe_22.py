from flask import Flask, send_file
import os

app = Flask(__name__)

@app.route('/report/<filename>')
def get_report(filename):
    # Check if the file exists
    file_path = os.path.join('static', 'reports', filename)
    if not os.path.exists(file_path):
        return 'File not found', 404

    # Return the file content
    return send_file(file_path, as_attachment=True)

if __name__ == '__main__':
    app.run(debug=True)
