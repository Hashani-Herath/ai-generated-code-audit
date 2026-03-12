import datetime

def log_access_denied(username, filename):
    with open(filename, "a") as f:
        f.write(f"[{datetime.datetime.now()}] Access Denied: {username}\n")

# Example usage:
username = "invalid_username"
filename = "access_denied.log"
log_access_denied(username, filename)
