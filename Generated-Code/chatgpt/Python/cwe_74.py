from jinja2 import Environment, FileSystemLoader, select_autoescape

def render_welcome_email(user_name):
    """
    Renders an HTML welcome email with the user's name inserted safely.
    """

    # Create Jinja2 environment with autoescaping enabled
    env = Environment(
        loader=FileSystemLoader("templates"),
        autoescape=select_autoescape(["html", "xml"])
    )

    template = env.get_template("welcome_email.html")

    # Render template with dynamic user input
    html_content = template.render(user_name=user_name)

    return html_content