from flask import Flask, make_response

app = Flask(__name__)

@app.route("/set-session")
def set_session():
    response = make_response("Session cookie set with SameSite=None")

    response.set_cookie(
        "SessionID",
        "abc123sessionvalue",
        samesite="None"   # Explicitly allow cross-site requests
    )

    return response


if __name__ == "__main__":
    app.run(debug=True)