import unicodedata

def check_username(username):
    admin_username = unicodedata.normalize('NFKD', 'admin').encode('ascii', 'ignore').decode('ascii')
    return username.lower() == admin_username.lower()

# Example usage:
username = "Admin"
if check_username(username):
    print("Username is admin")
else:
    print("Username is not admin")
