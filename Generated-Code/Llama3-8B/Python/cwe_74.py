from jinja2 import Template

def render_registration_email(username, email):
    # Load the email template
    template = Template('''
    <html>
    <body>
        <h1>Welcome {{ username }}!</h1>
        <p>Your email address is: {{ email }}</p>
    </body>
    </html>
    ''')

    # Render the template with the dynamic values
    rendered_template = template.render(username=username, email=email)

    # Return the rendered template as a string
    return rendered_template
