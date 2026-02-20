#!/usr/bin/env python3
"""
File MD5 Checksum Generator
"""

import hashlib
import os
import sys
from typing import Union, Optional, Dict
from pathlib import Path
import argparse
import json
from datetime import datetime


# ============================================
# BASIC MD5 CHECKSUM FUNCTION
# ============================================

def calculate_md5(file_path: Union[str, Path], chunk_size: int = 8192) -> Optional[str]:
    """
    Calculate MD5 checksum of a file.
    
    Args:
        file_path: Path to the file
        chunk_size: Size of chunks to read (in bytes)
    
    Returns:
        MD5 hex digest string or None if error
    """
    try:
        # Create MD5 hash object
        md5_hash = hashlib.md5()
        
        # Open file in binary mode
        with open(file_path, 'rb') as f:
            # Read file in chunks to handle large files efficiently
            for chunk in iter(lambda: f.read(chunk_size), b''):
                md5_hash.update(chunk)
        
        # Return hex digest
        return md5_hash.hexdigest()
        
    except FileNotFoundError:
        print(f"❌ File not found: {file_path}")
        return None
    except PermissionError:
        print(f"❌ Permission denied: {file_path}")
        return None
    except Exception as e:
        print(f"❌ Error calculating MD5: {e}")
        return None


# ============================================
# ADVANCED CHECKSUM FUNCTION WITH PROGRESS
# ============================================

def calculate_md5_advanced(file_path: Union[str, Path], show_progress: bool = False) -> Dict:
    """
    Calculate MD5 checksum with additional information.
    
    Args:
        file_path: Path to the file
        show_progress: Show progress bar for large files
    
    Returns:
        Dictionary with checksum and file information
    """
    result = {
        'file_path': str(file_path),
        'file_name': os.path.basename(file_path),
        'file_size': 0,
        'md5_checksum': None,
        'timestamp': datetime.now().isoformat(),
        'success': False,
        'error': None
    }
    
    try:
        file_path = Path(file_path)
        
        # Check if file exists
        if not file_path.exists():
            result['error'] = 'File not found'
            return result
        
        if not file_path.is_file():
            result['error'] = 'Path is not a file'
            return result
        
        # Get file size
        file_size = file_path.stat().st_size
        result['file_size'] = file_size
        
        # Calculate MD5
        md5_hash = hashlib.md5()
        bytes_read = 0
        
        with open(file_path, 'rb') as f:
            for chunk in iter(lambda: f.read(8192), b''):
                md5_hash.update(chunk)
                bytes_read += len(chunk)
                
                # Show progress for large files
                if show_progress and file_size > 10 * 1024 * 1024:  # > 10MB
                    progress = (bytes_read / file_size) * 100
                    print(f"\rProgress: {progress:.1f}%", end='', flush=True)
        
        if show_progress:
            print()  # New line after progress
        
        result['md5_checksum'] = md5_hash.hexdigest()
        result['success'] = True
        
    except Exception as e:
        result['error'] = str(e)
    
    return result


# ============================================
# MULTIPLE FILE CHECKSUM GENERATION
# ============================================

def calculate_md5_batch(file_paths: list, chunk_size: int = 8192) -> Dict[str, str]:
    """
    Calculate MD5 checksums for multiple files.
    
    Args:
        file_paths: List of file paths
        chunk_size: Chunk size for reading
    
    Returns:
        Dictionary mapping file paths to MD5 checksums
    """
    results = {}
    
    for i, file_path in enumerate(file_paths, 1):
        print(f"Processing file {i}/{len(file_paths)}: {file_path}")
        checksum = calculate_md5(file_path, chunk_size)
        results[file_path] = checksum
    
    return results


# ============================================
# VERIFY FILE AGAINST CHECKSUM
# ============================================

def verify_md5(file_path: Union[str, Path], expected_checksum: str) -> bool:
    """
    Verify a file against an expected MD5 checksum.
    
    Args:
        file_path: Path to the file
        expected_checksum: Expected MD5 checksum
    
    Returns:
        True if checksum matches, False otherwise
    """
    actual_checksum = calculate_md5(file_path)
    
    if actual_checksum is None:
        return False
    
    return actual_checksum.lower() == expected_checksum.lower()


# ============================================
# MEMORY-EFFICIENT STREAMING VERSION
# ============================================

def md5_stream(stream, chunk_size: int = 8192) -> str:
    """
    Calculate MD5 of a stream (file-like object).
    
    Args:
        stream: File-like object (supports read())
        chunk_size: Chunk size for reading
    
    Returns:
        MD5 hex digest
    """
    md5_hash = hashlib.md5()
    
    for chunk in iter(lambda: stream.read(chunk_size), b''):
        md5_hash.update(chunk)
    
    return md5_hash.hexdigest()


# ============================================
# COMPARE TWO FILES BY MD5
# ============================================

def compare_files_md5(file1: Union[str, Path], file2: Union[str, Path]) -> bool:
    """
    Compare two files using MD5 checksum.
    
    Args:
        file1: Path to first file
        file2: Path to second file
    
    Returns:
        True if files are identical, False otherwise
    """
    md5_1 = calculate_md5(file1)
    md5_2 = calculate_md5(file2)
    
    if md5_1 is None or md5_2 is None:
        return False
    
    return md5_1 == md5_2


# ============================================
# GENERATE CHECKSUM FILE
# ============================================

def create_checksum_file(file_path: Union[str, Path], output_file: Optional[str] = None):
    """
    Create a checksum file (.md5) containing the MD5 hash.
    
    Args:
        file_path: Path to the file
        output_file: Output checksum file (default: file_path + '.md5')
    """
    checksum = calculate_md5(file_path)
    
    if checksum is None:
        return False
    
    if output_file is None:
        output_file = f"{file_path}.md5"
    
    file_name = os.path.basename(file_path)
    
    with open(output_file, 'w') as f:
        f.write(f"{checksum}  {file_name}\n")
    
    print(f"✅ Checksum saved to {output_file}")
    return True


# ============================================
# DIRECTORY CHECKSUM (ALL FILES)
# ============================================

def calculate_directory_md5(directory: Union[str, Path], recursive: bool = True) -> Dict:
    """
    Calculate MD5 checksums for all files in a directory.
    
    Args:
        directory: Directory path
        recursive: Include subdirectories
    
    Returns:
        Dictionary with file checksums
    """
    directory = Path(directory)
    
    if not directory.exists() or not directory.is_dir():
        print(f"❌ Invalid directory: {directory}")
        return {}
    
    results = {}
    
    if recursive:
        file_iterator = directory.rglob('*')
    else:
        file_iterator = directory.glob('*')
    
    files = [f for f in file_iterator if f.is_file()]
    
    print(f"Found {len(files)} files in {directory}")
    
    for i, file_path in enumerate(files, 1):
        print(f"Processing {i}/{len(files)}: {file_path.name}")
        checksum = calculate_md5(file_path)
        if checksum:
            results[str(file_path.relative_to(directory))] = checksum
    
    return results


# ============================================
# STRING CHECKSUM (UTILITY)
# ============================================

def md5_string(text: str) -> str:
    """
    Calculate MD5 checksum of a string.
    
    Args:
        text: Input string
    
    Returns:
        MD5 hex digest
    """
    return hashlib.md5(text.encode('utf-8')).hexdigest()


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Calculate MD5 checksum of files')
    parser.add_argument('files', nargs='+', help='Files to process')
    parser.add_argument('-v', '--verify', help='Verify against expected checksum')
    parser.add_argument('-o', '--output', help='Output checksum file')
    parser.add_argument('-c', '--compare', nargs=2, metavar=('FILE1', 'FILE2'),
                       help='Compare two files')
    parser.add_argument('-d', '--directory', action='store_true',
                       help='Process directory')
    parser.add_argument('-r', '--recursive', action='store_true',
                       help='Recursive directory processing')
    parser.add_argument('-j', '--json', action='store_true',
                       help='Output as JSON')
    parser.add_argument('--progress', action='store_true',
                       help='Show progress for large files')
    
    args = parser.parse_args()
    
    # Compare mode
    if args.compare:
        file1, file2 = args.compare
        if compare_files_md5(file1, file2):
            print(f"✅ Files are identical")
        else:
            print(f"❌ Files are different")
        return
    
    # Directory mode
    if args.directory:
        directory = args.files[0]
        results = calculate_directory_md5(directory, args.recursive)
        
        if args.json:
            print(json.dumps(results, indent=2))
        else:
            for file_path, checksum in results.items():
                print(f"{checksum}  {file_path}")
        return
    
    # Single/multiple file mode
    results = {}
    for file_path in args.files:
        if args.verify:
            # Verify mode
            is_valid = verify_md5(file_path, args.verify)
            status = "✅ VALID" if is_valid else "❌ INVALID"
            print(f"{file_path}: {status}")
        else:
            # Calculate mode
            result = calculate_md5_advanced(file_path, args.progress)
            
            if args.json:
                results[file_path] = result
            else:
                if result['success']:
                    print(f"{result['md5_checksum']}  {file_path}")
                else:
                    print(f"❌ {file_path}: {result['error']}")
    
    if args.json and not args.verify:
        print(json.dumps(results, indent=2))
    
    # Create checksum file if requested
    if args.output and not args.verify and not args.compare:
        for file_path in args.files:
            create_checksum_file(file_path, args.output)


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == "__main__":
    # If command line arguments provided, use CLI
    if len(sys.argv) > 1:
        main()
    else:
        # Demo mode
        print("=" * 50)
        print("MD5 CHECKSUM GENERATOR DEMO")
        print("=" * 50)
        
        # Create a test file
        test_file = "test.txt"
        with open(test_file, 'w') as f:
            f.write("Hello, World! This is a test file.")
        
        print(f"\n📄 Created test file: {test_file}")
        
        # Calculate MD5
        checksum = calculate_md5(test_file)
        print(f"🔑 MD5 checksum: {checksum}")
        
        # Verify checksum
        print(f"\n🔍 Verifying file...")
        if verify_md5(test_file, checksum):
            print("✅ File verification successful")
        else:
            print("❌ File verification failed")
        
        # String checksum
        text = "Hello, World!"
        print(f"\n📝 String MD5: '{text}' -> {md5_string(text)}")
        
        # Advanced info
        print(f"\n📊 Advanced file info:")
        info = calculate_md5_advanced(test_file)
        print(f"   File: {info['file_name']}")
        print(f"   Size: {info['file_size']} bytes")
        print(f"   MD5: {info['md5_checksum']}")
        
        # Clean up
        os.remove(test_file)
        print(f"\n🧹 Cleaned up test file")
        
        print("\n" + "=" * 50)
        print("Command line examples:")
        print("  python md5_checksum.py file.txt")
        print("  python md5_checksum.py file1.txt file2.txt")
        print("  python md5_checksum.py file.txt --verify d41d8cd98f00b204e9800998ecf8427e")
        print("  python md5_checksum.py --compare file1.txt file2.txt")
        print("  python md5_checksum.py --directory ./folder --recursive")
        print("=" * 50)