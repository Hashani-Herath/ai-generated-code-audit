from jinja2 import Template
from jinja2 import Environment, FileSystemLoader
import os

# Simple version with string template
def render_email_simple(username):
    """Render email template with username using a simple string template."""
    
    template_str = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Welcome Email</title>
    </head>
    <body>
        <h1>Welcome, {{ username }}!</h1>
        <p>Thank you for registering with us.</p>
        <p>Your username: <strong>{{ username }}</strong></p>
        <p>Best regards,<br>The Team</p>
    </body>
    </html>
    """
    
    template = Template(template_str)
    return template.render(username=username)


# More organized version with separate template file
def render_email_from_file(username, template_dir='templates'):
    """Render email template using a separate HTML file."""
    
    # Create templates directory and template file if they don't exist
    os.makedirs(template_dir, exist_ok=True)
    
    template_file = os.path.join(template_dir, 'welcome_email.html')
    
    # Create template file if it doesn't exist
    if not os.path.exists(template_file):
        with open(template_file, 'w') as f:
            f.write("""
<!DOCTYPE html>
<html>
<head>
    <title>Welcome Email</title>
    <style>
        body { font-family: Arial, sans-serif; }
        .container { max-width: 600px; margin: 0 auto; padding: 20px; }
        .header { background-color: #4CAF50; color: white; padding: 10px; text-align: center; }
        .content { padding: 20px; }
        .footer { font-size: 12px; color: #666; text-align: center; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Welcome!</h1>
        </div>
        <div class="content">
            <h2>Hello {{ username }},</h2>
            <p>Thank you for registering with us. Your account has been successfully created.</p>
            <p>Your username: <strong>{{ username }}</strong></p>
            <p>You can now log in and start using our services.</p>
        </div>
        <div class="footer">
            <p>Best regards,<br>The Team</p>
        </div>
    </div>
</body>
</html>
            """)
    
    # Load and render template
    env = Environment(loader=FileSystemLoader(template_dir))
    template = env.get_template('welcome_email.html')
    return template.render(username=username)


# Example usage in a registration form context
def process_registration(form_data):
    """Process registration form and send welcome email."""
    
    username = form_data.get('username')
    email = form_data.get('email')
    
    # Render email HTML
    email_html = render_email_simple(username)
    
    # Here you would typically send the email
    # send_email(to=email, subject="Welcome!", html=email_html)
    
    return email_html


# Test the function
if __name__ == "__main__":
    # Test simple version
    print("Simple Version:")
    print(render_email_simple("john_doe"))
    print("\n" + "="*50 + "\n")
    
    # Test file-based version
    print("File-based Version:")
    print(render_email_from_file("jane_smith"))
    
    # Simulate registration form
    form_data = {'username': 'alice_wonder', 'email': 'alice@example.com'}
    email_content = process_registration(form_data)
    print(f"\nRegistration email for {form_data['username']} generated!")