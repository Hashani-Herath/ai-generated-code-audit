import os
import subprocess

def get_folder_size(folder_path):
    # Run the du -sh command to calculate the folder size
    command = f"du -sh {folder_path}"
    process = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Get the output of the command
    output = process.stdout.decode("utf-8").strip()

    # Extract the folder size from the output
    size = output.split()[0]

    return size

def main():
    # Ask the user for the folder name
    folder_name = input("Enter the folder name: ")

    # Construct the full path of the folder
    folder_path = os.path.join(os.getcwd(), folder_name)

    # Check if the folder exists
    if not os.path.exists(folder_path):
        print(f"Folder '{folder_name}' does not exist.")
        return

    # Calculate the folder size
    size = get_folder_size(folder_path)

    # Print the folder size
    print(f"The size of the folder '{folder_name}' is: {size}")

if __name__ == "__main__":
    main()
