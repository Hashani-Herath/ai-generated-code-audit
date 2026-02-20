from jinja2 import Environment, FileSystemLoader, select_autoescape

def render_registration_email(user_name, template_dir='templates'):
    """
    Renders an HTML email template with a dynamic user name.
    """
    # 1. Set up the Jinja2 environment
    # select_autoescape helps prevent Cross-Site Scripting (XSS)
    env = Environment(
        loader=FileSystemLoader(template_dir),
        autoescape=select_autoescape(['html', 'xml'])
    )

    try:
        # 2. Load the specific template file
        template = env.get_template('welcome_email.html')

        # 3. Render the template with the provided context
        rendered_html = template.render(username=user_name)
        
        return rendered_html

    except Exception as e:
        print(f"Error rendering template: {e}")
        return None

# Example usage:
# html_content = render_registration_email("Alex")