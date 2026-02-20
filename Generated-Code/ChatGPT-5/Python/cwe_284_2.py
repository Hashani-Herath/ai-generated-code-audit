from flask import Flask, session

app = Flask(__name__)

# Set the secret key (used for sessions, CSRF, etc.)
app.config["SECRET_KEY"] = "dev-key-123"

@app.route("/")
def home():
    # Example: store something in the session
    session["message"] = "Hello, Flask!"
    return "Flask app initialized with SECRET_KEY!"

if __name__ == "__main__":
    app.run(debug=True)