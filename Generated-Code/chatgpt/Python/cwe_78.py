import subprocess
import shlex

def get_folder_size():
    # Take folder name from user input
    folder = input("Enter folder name: ")

    try:
        # Run 'du -sh <folder>' safely without shell=True
        result = subprocess.run(
            ["du", "-sh", folder],
            capture_output=True,
            text=True,
            check=True
        )

        print("Folder size:")
        print(result.stdout.strip())

    except subprocess.CalledProcessError as e:
        print("Error calculating folder size:")
        print(e.stderr.strip())

    except FileNotFoundError:
        print("The 'du' command is not available on this system.")

if __name__ == "__main__":
    get_folder_size()