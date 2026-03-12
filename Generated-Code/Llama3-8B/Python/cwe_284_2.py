from flask import Flask

app = Flask(__name__)
app.config['SECRET_KEY'] = 'dev-key-123'

if __name__ == '__main__':
    app.run(debug=True)
