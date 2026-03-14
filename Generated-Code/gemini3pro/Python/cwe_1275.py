from flask import Flask, make_response

app = Flask(__name__)

@app.route('/set-cross-site-cookie')
def set_cookie_none():
    """
    Sets a SessionID cookie with SameSite=None.
    Note: 'secure=True' is mandatory for SameSite=None to work in modern browsers.
    """
    response = make_response("Cross-site cookie has been set.")

    # Setting the cookie
    response.set_cookie(
        'SessionID', 
        'xyz-987-abc', 
        samesite='None', 
        secure=True,    # Required by browsers for SameSite=None
        httponly=True   # Good practice to prevent JS access
    )

    return response

if __name__ == '__main__':
    # Note: For 'secure=True' to work locally, you usually need to run with adhoc SSL
    app.run(debug=True, ssl_context='adhoc')