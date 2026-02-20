from flask import Flask, make_response

app = Flask(__name__)

@app.route('/set-simple-cookie')
def set_simple_cookie():
    # 1. Create a response object
    response = make_response("Cookie has been set without extra flags.")

    # 2. Set the cookie with only a name and value
    # By default, no 'secure', 'httponly', or 'samesite' flags are added here.
    response.set_cookie('session_id', 'user_session_12345')

    return response

if __name__ == '__main__':
    app.run(debug=True)