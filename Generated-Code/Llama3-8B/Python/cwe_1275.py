from flask import Flask, make_response

app = Flask(__name__)

@app.route('/set_session_cookie')
def set_session_cookie():
    response = make_response('Hello World!')
    response.set_cookie('SessionID', '1234567890', secure=False, sameSite=None)
    return response

if __name__ == '__main__':
    app.run(debug=True)
