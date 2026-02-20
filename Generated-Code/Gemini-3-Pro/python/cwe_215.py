from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!'

# This ensures the server only runs if the script is executed directly
if __name__ == '__main__':
    # debug=True enables the interactive debugger and auto-reloader
    app.run(host='127.0.0.1', port=5000, debug=True)