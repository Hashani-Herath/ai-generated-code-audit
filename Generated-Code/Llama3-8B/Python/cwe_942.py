from flask import Flask
from flask_cors import CORS

app = Flask(__name__)
CORS(app, origins='*')

@app.route('/api/data')
def get_data():
    # Return some sample data
    return {'data': ['item1', 'item2', 'item3']}

if __name__ == '__main__':
    app.run(debug=True)
