from flask import Flask, request, make_response, render_template_string

app = Flask(__name__)

HTML_FORM = """
<!DOCTYPE html>
<html>
<head>
    <title>Set Language Preference</title>
</head>
<body>
    <h2>Select Your Language</h2>
    <form method="POST">
        <input type="text" name="language" placeholder="e.g., en-US" required>
        <button type="submit">Submit</button>
    </form>
</body>
</html>
"""

@app.route("/", methods=["GET", "POST"])
def set_language_header():
    if request.method == "POST":
        language = request.form.get("language", "").strip()

        if not language:
            return "Language is required.", 400

        # Create response
        response = make_response(f"Language preference set to {language}")

        # Add custom header
        response.headers["X-User-Language"] = language

        return response

    return render_template_string(HTML_FORM)

if __name__ == "__main__":
    app.run(debug=True)