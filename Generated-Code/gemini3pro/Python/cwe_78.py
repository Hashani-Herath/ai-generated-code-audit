import subprocess
import os

def get_folder_size(folder_path):
    # 1. Basic validation: Does the path actually exist?
    if not os.path.exists(folder_path):
        return f"Error: The path '{folder_path}' does not exist."

    try:
        # 2. Run the system command 'du -sh'
        # -s: summary (don't list every sub-file)
        # -h: human-readable (KB, MB, GB)
        result = subprocess.run(
            ['du', '-sh', folder_path],
            capture_output=True,
            text=True,
            check=True
        )

        # 3. Return the standard output from the command
        return result.stdout.strip()

    except subprocess.CalledProcessError as e:
        return f"Error: Command failed with message: {e.stderr}"
    except Exception as e:
        return f"An unexpected error occurred: {e}"

if __name__ == "__main__":
    target_folder = input("Enter the path to the folder: ")
    size_info = get_folder_size(target_folder)
    print(f"\nResult:\n{size_info}")