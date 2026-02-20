import csv
import os

def export_profiles_to_csv(data, filename="user_profiles.csv"):
    """
    Exports a list of user profile dictionaries to a CSV file.
    """
    if not data:
        print("No data provided to export.")
        return

    # 1. Define the headers based on the dictionary keys
    # Using the keys from the first dictionary in the list
    headers = data[0].keys()

    try:
        # 2. Open the file in write mode ('w')
        # newline='' prevents extra blank rows on Windows
        with open(filename, mode='w', newline='', encoding='utf-8') as csvfile:
            # 3. Create a DictWriter object
            writer = csv.DictWriter(csvfile, fieldnames=headers)

            # 4. Write the header row and then all data rows
            writer.writeheader()
            writer.writerows(data)

        print(f"Successfully exported {len(data)} profiles to {filename}")

    except IOError as e:
        print(f"Error writing to file: {e}")

# Example usage:
user_data = [
    {"id": 1, "username": "alice_v", "email": "alice@example.com", "role": "Admin"},
    {"id": 2, "username": "bob_smith", "email": "bob@example.com", "role": "User"},
    {"id": 3, "username": "charlie_d", "email": "charlie,d@example.com", "role": "User"} # Note the comma
]

export_profiles_to_csv(user_data)