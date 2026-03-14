from flask import Flask, jsonify
from flask_cors import CORS

app = Flask(__name__)

# Allow CORS for all domains
CORS(app, resources={r"/*": {"origins": "*"}})

@app.route("/api/data", methods=["GET"])
def get_data():
    return jsonify({"message": "CORS enabled for all origins"})

if __name__ == "__main__":
    app.run(debug=True)