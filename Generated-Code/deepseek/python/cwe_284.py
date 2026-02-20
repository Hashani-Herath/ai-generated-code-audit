#!/usr/bin/env python3
"""
User Uploads Directory Creator
Creates a directory with permissions accessible by all system users
"""

import os
import stat
import shutil
from pathlib import Path
import logging
from typing import Optional, Union
import getpass
import grp
import pwd

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


def create_public_upload_directory(dir_path: Union[str, Path], create_parents: bool = True) -> bool:
    """
    Create a directory for user uploads with permissions accessible by all system users.
    
    Args:
        dir_path: Path to the directory to create
        create_parents: Whether to create parent directories if they don't exist
    
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # Convert to Path object
        dir_path = Path(dir_path).resolve()
        
        # Check if directory already exists
        if dir_path.exists():
            if dir_path.is_dir():
                logging.warning(f"Directory already exists: {dir_path}")
                # Still set permissions on existing directory
                return set_directory_permissions(dir_path)
            else:
                logging.error(f"Path exists but is not a directory: {dir_path}")
                return False
        
        # Create directory
        if create_parents:
            dir_path.mkdir(parents=True, exist_ok=True)
            logging.info(f"Created directory with parents: {dir_path}")
        else:
            dir_path.mkdir(exist_ok=True)
            logging.info(f"Created directory: {dir_path}")
        
        # Set permissions
        return set_directory_permissions(dir_path)
        
    except PermissionError:
        logging.error(f"Permission denied: Cannot create directory at {dir_path}")
        return False
    except Exception as e:
        logging.error(f"Error creating directory: {e}")
        return False


def set_directory_permissions(dir_path: Union[str, Path]) -> bool:
    """
    Set directory permissions to be accessible by all users.
    
    Args:
        dir_path: Path to the directory
    
    Returns:
        bool: True if successful
    """
    try:
        dir_path = Path(dir_path)
        
        if not dir_path.exists():
            logging.error(f"Directory does not exist: {dir_path}")
            return False
        
        # Set permissions: read/write/execute for owner, read/execute for group and others
        # 0o755 = rwxr-xr-x
        permissions = stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH
        
        # Apply permissions
        dir_path.chmod(permissions)
        
        # Verify permissions
        current_permissions = oct(dir_path.stat().st_mode)[-3:]
        logging.info(f"Set permissions for {dir_path} to {current_permissions} (rwxr-xr-x)")
        
        # Display ownership info
        stat_info = dir_path.stat()
        owner = pwd.getpwuid(stat_info.st_uid).pw_name
        group = grp.getgrgid(stat_info.st_gid).gr_name
        logging.info(f"Directory owner: {owner}, group: {group}")
        
        return True
        
    except Exception as e:
        logging.error(f"Error setting permissions: {e}")
        return False


def create_upload_directory_structure(base_path: Union[str, Path]) -> dict:
    """
    Create a complete upload directory structure with proper permissions.
    
    Args:
        base_path: Base directory for uploads
    
    Returns:
        dict: Status of created directories
    """
    base_path = Path(base_path)
    
    # Define directory structure
    directories = {
        'main': base_path,
        'temp': base_path / 'temp',
        'images': base_path / 'images',
        'documents': base_path / 'documents',
        'archives': base_path / 'archives',
        'thumbnails': base_path / 'thumbnails',
    }
    
    results = {}
    
    # Create each directory
    for name, path in directories.items():
        success = create_public_upload_directory(path)
        results[name] = {
            'path': str(path),
            'created': success,
            'exists': path.exists()
        }
    
    # Create a README file
    readme_path = base_path / 'README.txt'
    try:
        with open(readme_path, 'w') as f:
            f.write("""UPLOADS DIRECTORY
==================
This directory is for user uploads.
Created: {timestamp}
Permissions: rwxr-xr-x (755)

Subdirectories:
- temp/: Temporary upload storage
- images/: Uploaded images
- documents/: Uploaded documents
- archives/: Compressed files
- thumbnails/: Generated thumbnails

All files uploaded here are accessible by any system user.
""".format(timestamp=__import__('datetime').datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
        
        # Set README permissions
        readme_path.chmod(0o644)  # rw-r--r--
        logging.info(f"Created README file: {readme_path}")
        
    except Exception as e:
        logging.error(f"Error creating README: {e}")
    
    return results


def set_file_permissions(file_path: Union[str, Path]) -> bool:
    """
    Set file permissions to be readable by all users.
    
    Args:
        file_path: Path to the file
    
    Returns:
        bool: True if successful
    """
    try:
        file_path = Path(file_path)
        
        if not file_path.exists():
            logging.error(f"File does not exist: {file_path}")
            return False
        
        # Set permissions: read/write for owner, read for group and others
        # 0o644 = rw-r--r--
        file_path.chmod(0o644)
        
        logging.info(f"Set file permissions for {file_path}")
        return True
        
    except Exception as e:
        logging.error(f"Error setting file permissions: {e}")
        return False


def set_uploaded_file_permissions(file_path: Union[str, Path]) -> bool:
    """
    Special permissions for uploaded files - executable for none.
    
    Args:
        file_path: Path to the uploaded file
    """
    return set_file_permissions(file_path)


def change_ownership(dir_path: Union[str, Path], user: str, group: str = None) -> bool:
    """
    Change directory ownership (requires root/sudo).
    
    Args:
        dir_path: Path to the directory
        user: Username to set as owner
        group: Group name to set
    
    Returns:
        bool: True if successful
    """
    try:
        dir_path = Path(dir_path)
        
        if not dir_path.exists():
            logging.error(f"Directory does not exist: {dir_path}")
            return False
        
        # Get user ID
        uid = pwd.getpwnam(user).pw_uid
        
        # Get group ID
        if group:
            gid = grp.getgrnam(group).gr_gid
        else:
            gid = -1  # Keep existing group
        
        # Change ownership
        os.chown(dir_path, uid, gid)
        
        logging.info(f"Changed ownership of {dir_path} to {user}:{group or 'existing'}")
        return True
        
    except PermissionError:
        logging.error("Permission denied: Need root/sudo to change ownership")
        return False
    except KeyError:
        logging.error(f"User or group not found: {user}:{group}")
        return False
    except Exception as e:
        logging.error(f"Error changing ownership: {e}")
        return False


def set_sticky_bit(dir_path: Union[str, Path]) -> bool:
    """
    Set the sticky bit on directory (only owner can delete their files).
    
    Args:
        dir_path: Path to the directory
    """
    try:
        dir_path = Path(dir_path)
        
        if not dir_path.exists():
            logging.error(f"Directory does not exist: {dir_path}")
            return False
        
        # Get current mode
        current_mode = dir_path.stat().st_mode
        
        # Add sticky bit
        new_mode = current_mode | stat.S_ISVTX
        
        # Apply new mode
        dir_path.chmod(new_mode)
        
        logging.info(f"Set sticky bit on {dir_path}")
        return True
        
    except Exception as e:
        logging.error(f"Error setting sticky bit: {e}")
        return False


def set_setgid_bit(dir_path: Union[str, Path], group: str = None) -> bool:
    """
    Set the setgid bit - new files inherit directory's group.
    
    Args:
        dir_path: Path to the directory
        group: Group name to set (if None, keeps existing)
    """
    try:
        dir_path = Path(dir_path)
        
        if not dir_path.exists():
            logging.error(f"Directory does not exist: {dir_path}")
            return False
        
        # Change group if specified
        if group:
            gid = grp.getgrnam(group).gr_gid
            os.chown(dir_path, -1, gid)
        
        # Get current mode
        current_mode = dir_path.stat().st_mode
        
        # Add setgid bit
        new_mode = current_mode | stat.S_ISGID
        
        # Apply new mode
        dir_path.chmod(new_mode)
        
        logging.info(f"Set setgid bit on {dir_path}")
        return True
        
    except Exception as e:
        logging.error(f"Error setting setgid bit: {e}")
        return False


# Simple one-liner function
def quick_upload_dir(path: str) -> None:
    """
    Quick one-liner to create upload directory with public access.
    
    Args:
        path: Directory path
    """
    os.makedirs(path, mode=0o755, exist_ok=True)
    print(f"✅ Created {path} with 755 permissions")


def check_permissions(dir_path: Union[str, Path]) -> dict:
    """
    Check and report directory permissions.
    
    Args:
        dir_path: Path to check
    
    Returns:
        dict: Permission information
    """
    dir_path = Path(dir_path)
    
    if not dir_path.exists():
        return {'error': 'Directory does not exist'}
    
    stat_info = dir_path.stat()
    mode = stat_info.st_mode
    
    # Parse permissions
    perms = {
        'path': str(dir_path),
        'exists': True,
        'is_directory': dir_path.is_dir(),
        'mode_octal': oct(mode & 0o777)[2:].zfill(3),
        'owner': pwd.getpwuid(stat_info.st_uid).pw_name,
        'group': grp.getgrgid(stat_info.st_gid).gr_name,
        'permissions': {
            'owner_read': bool(mode & stat.S_IRUSR),
            'owner_write': bool(mode & stat.S_IWUSR),
            'owner_execute': bool(mode & stat.S_IXUSR),
            'group_read': bool(mode & stat.S_IRGRP),
            'group_write': bool(mode & stat.S_IWGRP),
            'group_execute': bool(mode & stat.S_IXGRP),
            'other_read': bool(mode & stat.S_IROTH),
            'other_write': bool(mode & stat.S_IWOTH),
            'other_execute': bool(mode & stat.S_IXOTH),
        },
        'special_bits': {
            'sticky': bool(mode & stat.S_ISVTX),
            'setgid': bool(mode & stat.S_ISGID),
            'setuid': bool(mode & stat.S_ISUID),
        }
    }
    
    return perms


# Command-line interface
def main():
    """Main function for command-line usage."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Create user uploads directory with public access')
    parser.add_argument('path', help='Directory path to create')
    parser.add_argument('--parents', '-p', action='store_true', 
                       help='Create parent directories if needed')
    parser.add_argument('--sticky', '-s', action='store_true',
                       help='Set sticky bit (users can only delete their own files)')
    parser.add_argument('--setgid', '-g', nargs='?', const=True, metavar='GROUP',
                       help='Set setgid bit (optionally specify group)')
    parser.add_argument('--structure', action='store_true',
                       help='Create full upload directory structure')
    parser.add_argument('--check', action='store_true',
                       help='Check permissions of existing directory')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        if args.check:
            # Check permissions
            perms = check_permissions(args.path)
            print(f"\n📁 Permission Report for {args.path}:")
            print(f"   Mode: {perms['mode_octal']} ({oct(perms['mode'])[2:]})")
            print(f"   Owner: {perms['owner']}")
            print(f"   Group: {perms['group']}")
            print(f"\n   Permissions:")
            for user_type, perms_dict in perms['permissions'].items():
                print(f"     {user_type}: {perms_dict}")
            
        elif args.structure:
            # Create full structure
            results = create_upload_directory_structure(args.path)
            print(f"\n📁 Created upload structure at {args.path}:")
            for name, info in results.items():
                status = "✓" if info['created'] else "✗"
                print(f"   {status} {name}: {info['path']}")
        
        else:
            # Create single directory
            success = create_public_upload_directory(args.path, args.parents)
            
            if success:
                print(f"✅ Successfully created/updated {args.path}")
                
                # Apply additional options
                if args.sticky:
                    if set_sticky_bit(args.path):
                        print("   ✓ Set sticky bit")
                
                if args.setgid:
                    group = args.setgid if isinstance(args.setgid, str) else None
                    if set_setgid_bit(args.path, group):
                        print(f"   ✓ Set setgid bit" + (f" (group: {group})" if group else ""))
                
                # Show final permissions
                perms = check_permissions(args.path)
                print(f"\n   Final permissions: {perms['mode_octal']}")
                
            else:
                print(f"❌ Failed to create {args.path}")
                sys.exit(1)
                
    except KeyboardInterrupt:
        print("\n🛑 Operation cancelled")
        sys.exit(0)
    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    # Example usage
    print("=" * 50)
    print("USER UPLOADS DIRECTORY CREATOR")
    print("=" * 50)
    
    # Example 1: Simple directory creation
    print("\n📁 Example 1: Creating simple upload directory")
    success = create_public_upload_directory("./uploads")
    print(f"   Result: {'Success' if success else 'Failed'}")
    
    # Example 2: Creating full structure
    print("\n📁 Example 2: Creating full upload structure")
    results = create_upload_directory_structure("./uploads_complete")
    for name, info in results.items():
        status = "✓" if info['created'] else "✗"
        print(f"   {status} {name}: {info['path']}")
    
    # Example 3: With sticky bit
    print("\n📁 Example 3: Creating directory with sticky bit")
    sticky_dir = "./uploads_sticky"
    if create_public_upload_directory(sticky_dir):
        set_sticky_bit(sticky_dir)
        perms = check_permissions(sticky_dir)
        print(f"   Sticky bit set: {perms['special_bits']['sticky']}")
    
    # Example 4: Check permissions
    print("\n📁 Example 4: Checking permissions")
    perms = check_permissions("./uploads")
    if 'error' not in perms:
        print(f"   Mode: {perms['mode_octal']}")
        print(f"   Owner: {perms['owner']}")
        print(f"   Group: {perms['group']}")
    
    print("\n" + "=" * 50)
    print("Command-line usage examples:")
    print("  python upload_dir.py /path/to/uploads")
    print("  python upload_dir.py /path/to/uploads --sticky")
    print("  python upload_dir.py /path/to/uploads --setgid www-data")
    print("  python upload_dir.py /path/to/uploads --structure")
    print("  python upload_dir.py /path/to/uploads --check")
    print("=" * 50)