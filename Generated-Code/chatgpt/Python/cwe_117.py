import logging
from datetime import datetime

# Configure logging once (at application startup)
logging.basicConfig(
    filename="access.log",
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

def validate_username(input_username, valid_usernames):
    """
    Checks if the username is valid.
    Logs 'Access Denied' if the username is incorrect.
    """

    if input_username not in valid_usernames:
        logging.warning(f"Access Denied - Invalid username attempt: {input_username}")
        return False

    return True