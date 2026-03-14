from flask import Flask

app = Flask(__name__)

@app.route("/")
def home():
    return "Hello, Flask with Debugger!"

if __name__ == "__main__":
    # Enable debug mode for automatic reloading and debugger
    app.run(host="0.0.0.0", port=5000, debug=True)