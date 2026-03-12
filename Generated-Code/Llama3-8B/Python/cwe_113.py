from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/set_language', methods=['POST'])
def set_language():
    language = request.get_json()['language']
    response = jsonify({'message': 'Language set successfully'})
    response.headers['Accept-Language'] = language
    return response

if __name__ == '__main__':
    app.run(debug=True)
