from flask import Flask, request, make_response, jsonify, render_template_string
import datetime

app = Flask(__name__)

# Simple HTML form for testing
HTML_FORM = '''
<!DOCTYPE html>
<html>
<head>
    <title>Language Preference</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 600px; margin: 0 auto; }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        select, input[type=text] { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }
        .response { background: #f0f0f0; padding: 15px; border-radius: 4px; margin-top: 20px; }
        .header-info { font-family: monospace; background: #333; color: #fff; padding: 10px; border-radius: 4px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌐 Language Preference Header</h1>
        
        <form method="POST" action="/set-language">
            <div class="form-group">
                <label for="language">Select your language:</label>
                <select name="language" id="language" required>
                    <option value="en-US">English (US)</option>
                    <option value="en-GB">English (UK)</option>
                    <option value="es-ES">Spanish (Spain)</option>
                    <option value="es-MX">Spanish (Mexico)</option>
                    <option value="fr-FR">French</option>
                    <option value="de-DE">German</option>
                    <option value="it-IT">Italian</option>
                    <option value="pt-BR">Portuguese (Brazil)</option>
                    <option value="zh-CN">Chinese (Simplified)</option>
                    <option value="ja-JP">Japanese</option>
                    <option value="ko-KR">Korean</option>
                    <option value="ru-RU">Russian</option>
                    <option value="ar-SA">Arabic</option>
                    <option value="hi-IN">Hindi</option>
                </select>
            </div>
            
            <div class="form-group">
                <label for="custom">Or enter custom language code:</label>
                <input type="text" name="custom_language" id="custom" placeholder="e.g., fr-CA, sw-KE">
            </div>
            
            <button type="submit">Set Language Header</button>
        </form>
        
        <div class="response">
            <h3>Last Response Headers:</h3>
            <div class="header-info" id="headers">
                {{ headers | safe }}
            </div>
        </div>
    </div>
</body>
</html>
'''


# 1. Basic route - sets Accept-Language header
@app.route('/set-language', methods=['POST'])
def set_language_header():
    """
    Set Accept-Language header based on user preference.
    """
    # Get language from form
    language = request.form.get('language')
    custom_language = request.form.get('custom_language')
    
    # Use custom language if provided, otherwise use selected
    if custom_language and custom_language.strip():
        language = custom_language.strip()
    
    if not language:
        return "Language preference is required", 400
    
    # Create response
    response = make_response(f"""
    <h1>Language Preference Set</h1>
    <p>Your language preference: <strong>{language}</strong></p>
    <p>The <code>Accept-Language</code> header has been added to the response.</p>
    <p><a href="/">Check headers</a> | <a href="/">Go back</a></p>
    """)
    
    # Add language header
    response.headers['Accept-Language'] = language
    
    return response


# 2. More comprehensive - adds Content-Language header
@app.route('/set-content-language', methods=['POST'])
def set_content_language():
    """
    Set Content-Language header for the response content.
    """
    language = request.form.get('language', 'en-US')
    
    # Create response with content in specified language
    content = get_content_in_language(language)
    
    response = make_response(content)
    response.headers['Content-Language'] = language
    response.headers['Vary'] = 'Accept-Language'  # Indicate caching varies by language
    
    return response


# 3. API endpoint - returns JSON with language header
@app.route('/api/set-language', methods=['POST'])
def api_set_language():
    """
    API endpoint to set language header.
    """
    # Get language from JSON or form
    if request.is_json:
        data = request.get_json()
        language = data.get('language', 'en-US')
    else:
        language = request.form.get('language', 'en-US')
    
    response = jsonify({
        'status': 'success',
        'message': f'Language header set to {language}',
        'language': language,
        'timestamp': datetime.datetime.utcnow().isoformat()
    })
    
    response.headers['Accept-Language'] = language
    response.headers['Content-Language'] = language
    
    return response


# 4. Route that reads Accept-Language from request
@app.route('/')
def index():
    """
    Home page showing language detection.
    """
    # Read language from request headers
    browser_language = request.headers.get('Accept-Language', 'Not provided')
    
    # Get headers to display
    headers = dict(request.headers)
    
    # Format headers for display
    header_html = '<br>'.join([f'<strong>{k}:</strong> {v}' for k, v in headers.items()])
    
    return render_template_string(HTML_FORM, headers=header_html)


# 5. Advanced route with language negotiation
@app.route('/negotiate-language')
def negotiate_language():
    """
    Advanced language negotiation based on user preference and available languages.
    """
    # Available languages in your application
    available_languages = ['en-US', 'es-ES', 'fr-FR', 'de-DE', 'it-IT', 'pt-BR', 'ja-JP']
    
    # Get user's preferred language from various sources
    user_preference = (
        request.args.get('lang') or  # URL parameter
        request.cookies.get('preferred_language') or  # Cookie
        request.headers.get('Accept-Language', 'en-US').split(',')[0].split(';')[0]  # Browser
    )
    
    # Validate and negotiate language
    if user_preference in available_languages:
        selected_language = user_preference
    else:
        # Fallback to English
        selected_language = 'en-US'
    
    # Create response with content in negotiated language
    content = get_localized_content(selected_language)
    
    response = make_response(content)
    
    # Set language headers
    response.headers['Content-Language'] = selected_language
    response.headers['X-Negotiated-Language'] = selected_language
    response.headers['Vary'] = 'Accept-Language, Cookie'
    
    # Also set a cookie for future requests
    response.set_cookie('preferred_language', selected_language, max_age=30*24*3600)
    
    return response


# 6. Route that sets multiple language-related headers
@app.route('/set-language-headers', methods=['POST'])
def set_multiple_language_headers():
    """
    Set multiple language-related headers.
    """
    language = request.form.get('language', 'en-US')
    
    response = make_response(render_language_page(language))
    
    # Set various language-related headers
    response.headers.update({
        'Accept-Language': language,  # What client accepts
        'Content-Language': language,  # What content is in
        'X-Content-Language': language,  # Custom header
        'Vary': 'Accept-Language',  # For caching
        'Cache-Control': 'private, max-age=3600'  # Cache per user
    })
    
    return response


# 7. Decorator for adding language header to multiple routes
def add_language_header(language_param='lang'):
    """
    Decorator to add language header to route responses.
    
    Args:
        language_param: Parameter name to get language from
    """
    def decorator(f):
        def wrapped(*args, **kwargs):
            # Get language from request
            if request.method == 'GET':
                language = request.args.get(language_param, 'en-US')
            else:
                language = request.form.get(language_param, 'en-US')
            
            # Execute route function
            response = f(*args, **kwargs)
            
            # If response is a tuple (response, status_code, headers)
            if isinstance(response, tuple):
                resp_obj, status_code, headers = response
                if not isinstance(resp_obj, flask.Response):
                    resp_obj = make_response(resp_obj)
                resp_obj.headers['Content-Language'] = language
                return resp_obj, status_code, headers
            
            # If response is a Response object
            if not isinstance(response, flask.Response):
                response = make_response(response)
            
            response.headers['Content-Language'] = language
            return response
        
        wrapped.__name__ = f.__name__
        return wrapped
    return decorator


# Example usage of decorator
@app.route('/about')
@add_language_header('lang')
def about_page():
    """About page with automatic language header."""
    return """
    <h1>About Us</h1>
    <p>This page automatically gets a Content-Language header.</p>
    """


# Helper functions
def get_content_in_language(language):
    """Return content based on language preference."""
    content = {
        'en-US': '<h1>Welcome!</h1><p>Content in English</p>',
        'es-ES': '<h1>¡Bienvenido!</h1><p>Contenido en español</p>',
        'fr-FR': '<h1>Bienvenue!</h1><p>Contenu en français</p>',
        'de-DE': '<h1>Willkommen!</h1><p>Inhalt auf Deutsch</p>',
    }
    return content.get(language, content['en-US'])


def get_localized_content(language):
    """Get localized content with navigation."""
    welcome_messages = {
        'en-US': 'Welcome!',
        'es-ES': '¡Bienvenido!',
        'fr-FR': 'Bienvenue!',
        'de-DE': 'Willkommen!',
        'it-IT': 'Benvenuto!',
        'pt-BR': 'Bem-vindo!',
        'ja-JP': 'ようこそ！',
    }
    
    welcome = welcome_messages.get(language, welcome_messages['en-US'])
    
    return f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Localized Page</title>
        <style>
            body {{ font-family: Arial, sans-serif; margin: 40px; }}
            .info {{ background: #f0f0f0; padding: 20px; border-radius: 5px; }}
        </style>
    </head>
    <body>
        <h1>{welcome}</h1>
        <div class="info">
            <h3>Language Information:</h3>
            <p><strong>Selected Language:</strong> {language}</p>
            <p><strong>Browser Language:</strong> {request.headers.get('Accept-Language', 'Unknown')}</p>
            <p><strong>Content-Language Header:</strong> {language}</p>
        </div>
        <p><a href="/negotiate-language?lang=en-US">English</a> | 
           <a href="/negotiate-language?lang=es-ES">Español</a> | 
           <a href="/negotiate-language?lang=fr-FR">Français</a></p>
    </body>
    </html>
    """


def render_language_page(language):
    """Render a page showing language headers."""
    return f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Language Headers Set</title>
        <style>
            body {{ font-family: Arial, sans-serif; margin: 40px; }}
            .headers {{ background: #333; color: #fff; padding: 20px; border-radius: 5px; }}
        </style>
    </head>
    <body>
        <h1>Language Headers Set Successfully</h1>
        
        <h3>Headers added to this response:</h3>
        <div class="headers">
            <strong>Accept-Language:</strong> {language}<br>
            <strong>Content-Language:</strong> {language}<br>
            <strong>X-Content-Language:</strong> {language}<br>
            <strong>Vary:</strong> Accept-Language<br>
            <strong>Cache-Control:</strong> private, max-age=3600
        </div>
        
        <p><a href="/">Check all headers</a></p>
    </body>
    </html>
    """


# 8. Route for checking all headers
@app.route('/headers')
def show_headers():
    """Display all headers from the current request."""
    headers = dict(request.headers)
    return jsonify({
        'request_headers': headers,
        'message': 'These are the headers your browser sent'
    })


if __name__ == '__main__':
    app.run(debug=True, port=5000)