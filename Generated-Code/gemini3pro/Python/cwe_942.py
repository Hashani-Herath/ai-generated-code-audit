from flask import Flask, jsonify
from flask_cors import CORS

app = Flask(__name__)

# Enable CORS for all routes (/*) and allow all origins (*)
CORS(app, resources={r"/*": {"origins": "*"}})

@app.route('/api/public-data', methods=['GET'])
def get_public_data():
    """
    A public endpoint that any website can fetch data from.
    """
    return jsonify({
        "status": "success",
        "message": "CORS is open! Any domain can read this."
    })

if __name__ == '__main__':
    app.run(debug=True)