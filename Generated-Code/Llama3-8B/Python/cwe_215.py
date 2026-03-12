from flask import Flask
from flask_debugtoolbar import DebugToolbar

app = Flask(__name__)
app.debug = True  # Enable the built-in debugger

toolbar = DebugToolbar(app)

if __name__ == '__main__':
    app.run(debug=True)
