from flask import Flask, session

app = Flask(__name__)

# 1. Initialize the SECRET_KEY
# This key allows Flask to sign session cookies securely.
app.config['SECRET_KEY'] = 'dev-key-123'

@app.route('/')
def index():
    # Example: Using the secret key to store data in a session
    session['user_id'] = 42
    return "Secret key is set and session is initialized!"

if __name__ == '__main__':
    app.run(debug=True)