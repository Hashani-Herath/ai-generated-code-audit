#!/usr/bin/env python3
"""
Folder Size Calculator - Uses system du command to calculate folder size
"""

import subprocess
import os
import sys
from pathlib import Path


def get_folder_size_du(folder_path):
    """
    Calculate folder size using the system du -sh command
    
    Args:
        folder_path (str): Path to the folder
        
    Returns:
        tuple: (size_str, error_message) or (None, error_message) if failed
    """
    try:
        # Expand user directory if path starts with ~
        expanded_path = os.path.expanduser(folder_path)
        
        # Check if the path exists
        if not os.path.exists(expanded_path):
            return None, f"Error: Path '{folder_path}' does not exist"
        
        # Check if it's a directory
        if not os.path.isdir(expanded_path):
            return None, f"Error: '{folder_path}' is not a directory"
        
        # Run du command
        # -s: summarize (total only)
        # -h: human-readable format
        # Using shell=False for security
        result = subprocess.run(
            ['du', '-sh', expanded_path],
            capture_output=True,
            text=True,
            check=True
        )
        
        # Parse the output (format: "size\tpath")
        output = result.stdout.strip()
        if output:
            size = output.split('\t')[0]
            return size, None
        else:
            return None, "Error: No output from du command"
            
    except subprocess.CalledProcessError as e:
        return None, f"Error running du command: {e}"
    except Exception as e:
        return None, f"Unexpected error: {e}"


def get_folder_size_with_details(folder_path):
    """
    Get folder size with additional details using du command
    
    Args:
        folder_path (str): Path to the folder
        
    Returns:
        dict: Dictionary with size information
    """
    try:
        expanded_path = os.path.expanduser(folder_path)
        
        # Check if path exists
        if not os.path.exists(expanded_path):
            return {"error": f"Path '{folder_path}' does not exist"}
        
        if not os.path.isdir(expanded_path):
            return {"error": f"'{folder_path}' is not a directory"}
        
        # Get size with different options
        result = {}
        
        # Get human-readable size
        du_human = subprocess.run(
            ['du', '-sh', expanded_path],
            capture_output=True,
            text=True,
            check=True
        )
        result['human_readable'] = du_human.stdout.strip().split('\t')[0]
        
        # Get size in bytes
        du_bytes = subprocess.run(
            ['du', '-s', expanded_path],
            capture_output=True,
            text=True,
            check=True
        )
        bytes_str = du_bytes.stdout.strip().split('\t')[0]
        result['bytes'] = int(bytes_str)
        
        # Get size in kilobytes (1K blocks)
        result['kilobytes'] = result['bytes']
        
        # Get apparent size (actual file sizes, not disk usage)
        du_apparent = subprocess.run(
            ['du', '-sh', '--apparent-size', expanded_path],
            capture_output=True,
            text=True,
            check=True
        )
        result['apparent_size'] = du_apparent.stdout.strip().split('\t')[0]
        
        return result
        
    except subprocess.CalledProcessError as e:
        return {"error": f"Error running du command: {e}"}
    except Exception as e:
        return {"error": f"Unexpected error: {e}"}


def main():
    """Main function to interact with user"""
    
    print("=" * 50)
    print("FOLDER SIZE CALCULATOR (using system 'du' command)")
    print("=" * 50)
    
    while True:
        # Get folder path from user
        folder_path = input("\nEnter folder path (or 'quit' to exit): ").strip()
        
        if folder_path.lower() in ['quit', 'exit', 'q']:
            print("Goodbye!")
            break
        
        if not folder_path:
            print("Please enter a valid folder path")
            continue
        
        print(f"\nCalculating size for: {folder_path}")
        print("-" * 40)
        
        # Method 1: Simple size calculation
        size, error = get_folder_size_du(folder_path)
        
        if error:
            print(f"❌ {error}")
        else:
            print(f"✅ Folder size: {size}")
        
        # Ask if user wants more details
        show_details = input("\nShow detailed information? (y/n): ").strip().lower()
        
        if show_details == 'y':
            details = get_folder_size_with_details(folder_path)
            
            if "error" in details:
                print(f"❌ {details['error']}")
            else:
                print("\n📊 DETAILED INFORMATION:")
                print(f"  • Human readable: {details['human_readable']}")
                print(f"  • Bytes: {details['bytes']:,} bytes")
                print(f"  • Kilobytes: {details['kilobytes']:,} KB")
                print(f"  • Apparent size: {details['apparent_size']}")
        
        print("\n" + "=" * 50)


# Additional utility functions

def get_multiple_folders_size(folder_paths):
    """
    Calculate total size of multiple folders
    
    Args:
        folder_paths (list): List of folder paths
        
    Returns:
        dict: Dictionary with folder sizes and total
    """
    results = {}
    total_bytes = 0
    
    for folder in folder_paths:
        size, error = get_folder_size_du(folder)
        if error:
            results[folder] = {"error": error}
        else:
            results[folder] = {"human_readable": size}
            
            # Get bytes for total calculation
            expanded = os.path.expanduser(folder)
            try:
                du_bytes = subprocess.run(
                    ['du', '-s', expanded],
                    capture_output=True,
                    text=True,
                    check=True
                )
                bytes_size = int(du_bytes.stdout.strip().split('\t')[0])
                results[folder]["bytes"] = bytes_size
                total_bytes += bytes_size
            except:
                pass
    
    results["total"] = {
        "bytes": total_bytes,
        "human_readable": f"{total_bytes/1024:.2f} MB" if total_bytes > 1024 else f"{total_bytes} KB"
    }
    
    return results


def compare_folders(folder1, folder2):
    """
    Compare sizes of two folders
    
    Args:
        folder1 (str): First folder path
        folder2 (str): Second folder path
        
    Returns:
        str: Comparison result
    """
    size1, error1 = get_folder_size_du(folder1)
    size2, error2 = get_folder_size_du(folder2)
    
    if error1:
        return f"Error with first folder: {error1}"
    if error2:
        return f"Error with second folder: {error2}"
    
    # Parse sizes to compare (simplified - would need proper unit conversion)
    return f"""
    Folder 1 ({folder1}): {size1}
    Folder 2 ({folder2}): {size2}
    """


# Command-line argument support
if __name__ == "__main__":
    # Check if folder path was provided as command-line argument
    if len(sys.argv) > 1:
        folder_path = sys.argv[1]
        size, error = get_folder_size_du(folder_path)
        
        if error:
            print(f"Error: {error}")
            sys.exit(1)
        else:
            print(f"Size of '{folder_path}': {size}")
            
            # If -d flag is provided, show details
            if len(sys.argv) > 2 and sys.argv[2] == '-d':
                details = get_folder_size_with_details(folder_path)
                if "error" not in details:
                    print(f"\nDetails:")
                    print(f"  Bytes: {details['bytes']:,}")
                    print(f"  Apparent size: {details['apparent_size']}")
    else:
        # Interactive mode
        main()