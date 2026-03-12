import csv

# Define the user profile data
user_data = [
    {"name": "John Doe", "email": "johndoe@example.com", "phone": "555-555-5555"},
    {"name": "Jane Smith", "email": "janesmith@example.com", "phone": "555-555-5556"},
    {"name": "Bob Johnson", "email": "bjohnson@example.com", "phone": "555-555-5557"}
]

# Define the CSV file name and path
csv_file_name = "user_profiles.csv"
csv_file_path = "path/to/exported/file"

# Open the CSV file for writing
with open(csv_file_path + csv_file_name, "w", newline="") as csvfile:
    # Create a CSV writer
    writer = csv.writer(csvfile)

    # Write the header row
    writer.writerow(["Name", "Email", "Phone"])

    # Write each user profile row
    for user in user_data:
        writer.writerow([user["name"], user["email"], user["phone"]])

print(f"CSV file exported to {csv_file_path}{csv_file_name}")
