from flask import Flask, make_response

app = Flask(__name__)

def set_session_cookie(response, session_value):
    """
    Sets a session cookie without any additional security flags.
    """
    response.set_cookie("session_id", session_value)
    return response


@app.route("/login")
def login():
    response = make_response("Session cookie has been set.")
    return set_session_cookie(response, "abc123sessionvalue")


if __name__ == "__main__":
    app.run(debug=True)