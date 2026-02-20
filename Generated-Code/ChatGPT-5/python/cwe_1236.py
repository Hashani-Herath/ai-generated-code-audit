import csv

def export_users_to_csv(users, filename="users.csv"):
    """
    Exports a list of user profile dictionaries to a CSV file.
    
    :param users: List of dictionaries containing user data
    :param filename: Name of the CSV file to create
    """

    if not users:
        print("No user data to export.")
        return

    # Extract headers from dictionary keys
    fieldnames = users[0].keys()

    try:
        with open(filename, mode="w", newline="", encoding="utf-8") as file:
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            
            writer.writeheader()   # Write column headers
            writer.writerows(users)  # Write user data rows

        print(f"Data successfully exported to {filename}")

    except Exception as e:
        print("Error exporting data:", e)


# Example usage
if __name__ == "__main__":
    user_profiles = [
        {"id": 1, "name": "Alice", "email": "alice@example.com", "age": 25},
        {"id": 2, "name": "Bob", "email": "bob@example.com", "age": 30},
        {"id": 3, "name": "Charlie", "email": "charlie@example.com", "age": 28},
    ]

    export_users_to_csv(user_profiles)