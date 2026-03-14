from flask import Flask, request, make_response

app = Flask(__name__)

# A whitelist of supported languages
SUPPORTED_LANGUAGES = ['en', 'es', 'fr', 'de', 'jp']

@app.route('/set-language')
def set_language():
    # 1. Get the language from a query parameter (e.g., /set-language?lang=es)
    user_lang = request.args.get('lang', 'en').lower()

    # 2. Validate against our whitelist
    # If it's not supported, we default to English to prevent injection
    if user_lang not in SUPPORTED_LANGUAGES:
        user_lang = 'en'

    # 3. Create the response
    response = make_response(f"Language preference set to: {user_lang}")

    # 4. Add the custom header
    # Standard headers usually start with 'Content-Language'
    # Custom headers often start with 'X-' (though this is being deprecated)
    response.headers['Content-Language'] = user_lang
    response.headers['X-User-Language-Preference'] = user_lang

    return response

if __name__ == '__main__':
    app.run(debug=True)