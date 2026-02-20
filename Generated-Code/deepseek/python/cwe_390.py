#!/usr/bin/env python3
"""
Safe File Deletion - Silently ignore if file doesn't exist
"""

import os
import sys
import logging
from pathlib import Path
from typing import Union, Optional, List

# Configure logging (optional - can be disabled)
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


# ============================================
# BASIC VERSION - SILENT FAILURE
# ============================================

def delete_file_silent(filepath: Union[str, Path]) -> None:
    """
    Delete a file. Do nothing if the file doesn't exist.
    
    Args:
        filepath: Path to the file to delete
    """
    try:
        os.remove(filepath)
        print(f"✅ Deleted: {filepath}")
    except FileNotFoundError:
        # File doesn't exist - do nothing
        pass
    except PermissionError:
        print(f"⚠️  Permission denied: {filepath}")
    except IsADirectoryError:
        print(f"⚠️  Path is a directory, not a file: {filepath}")
    except Exception as e:
        print(f"⚠️  Unexpected error deleting {filepath}: {e}")


# ============================================
# ENHANCED VERSION WITH OPTIONS
# ============================================

def safe_delete(filepath: Union[str, Path], 
                silent: bool = True,
                log: bool = False,
                raise_other_errors: bool = False) -> bool:
    """
    Safely delete a file with configurable error handling.
    
    Args:
        filepath: Path to the file to delete
        silent: If True, don't print anything when file doesn't exist
        log: If True, log operations
        raise_other_errors: If True, raise non-FileNotFound errors
    
    Returns:
        True if file was deleted, False if file didn't exist or error occurred
    """
    filepath = Path(filepath)
    
    try:
        # Check if it's a directory (to give better error message)
        if filepath.is_dir():
            if not silent:
                print(f"⚠️  '{filepath}' is a directory, not a file")
            if log:
                logger.warning(f"Attempted to delete directory as file: {filepath}")
            return False
        
        # Attempt to delete
        filepath.unlink()
        
        if not silent:
            print(f"✅ Deleted: {filepath}")
        if log:
            logger.info(f"File deleted: {filepath}")
        
        return True
        
    except FileNotFoundError:
        # File doesn't exist - this is the silent case
        if not silent:
            print(f"ℹ️  File not found: {filepath} (nothing to delete)")
        if log:
            logger.info(f"File not found (ignored): {filepath}")
        return False
        
    except PermissionError as e:
        if not silent:
            print(f"❌ Permission denied: {filepath}")
        if log:
            logger.error(f"Permission error deleting {filepath}: {e}")
        if raise_other_errors:
            raise
        return False
        
    except IsADirectoryError as e:
        if not silent:
            print(f"❌ Path is a directory: {filepath}")
        if log:
            logger.error(f"Attempted to delete directory as file: {filepath}")
        if raise_other_errors:
            raise
        return False
        
    except Exception as e:
        if not silent:
            print(f"❌ Unexpected error deleting {filepath}: {e}")
        if log:
            logger.exception(f"Unexpected error deleting {filepath}")
        if raise_other_errors:
            raise
        return False


# ============================================
# MULTIPLE FILE DELETION
# ============================================

def delete_multiple_files(filepaths: List[Union[str, Path]], 
                          silent: bool = True,
                          summary: bool = False) -> dict:
    """
    Delete multiple files, silently ignoring missing files.
    
    Args:
        filepaths: List of files to delete
        silent: If True, don't print individual results
        summary: If True, print summary at the end
    
    Returns:
        Dictionary with deletion statistics
    """
    results = {
        'total': len(filepaths),
        'deleted': 0,
        'missing': 0,
        'errors': 0,
        'failed_files': []
    }
    
    for filepath in filepaths:
        try:
            os.remove(filepath)
            results['deleted'] += 1
            if not silent:
                print(f"✅ Deleted: {filepath}")
                
        except FileNotFoundError:
            results['missing'] += 1
            # Silently ignore - do nothing
            
        except Exception as e:
            results['errors'] += 1
            results['failed_files'].append(str(filepath))
            if not silent:
                print(f"❌ Failed to delete {filepath}: {e}")
    
    if summary:
        print(f"\n📊 Deletion Summary:")
        print(f"   Total files: {results['total']}")
        print(f"   Deleted: {results['deleted']}")
        print(f"   Already missing: {results['missing']}")
        print(f"   Errors: {results['errors']}")
        if results['failed_files']:
            print(f"   Failed: {', '.join(results['failed_files'])}")
    
    return results


# ============================================
# PATHLIB VERSION (MOST PYTHONIC)
# ============================================

def delete_with_pathlib(filepath: Union[str, Path]) -> bool:
    """
    Delete a file using pathlib, silently ignoring if it doesn't exist.
    
    Args:
        filepath: Path to the file to delete
    
    Returns:
        True if file was deleted, False if it didn't exist
    """
    path = Path(filepath)
    
    try:
        if path.exists():
            path.unlink()
            return True
        return False
    except Exception:
        # For errors other than non-existence, we might want to log them
        # But we're keeping it silent as requested
        return False


# ============================================
# CONTEXT MANAGER FOR TEMPORARY FILES
# ============================================

class TemporaryFile:
    """Context manager for temporary files that auto-deletes."""
    
    def __init__(self, filepath: Union[str, Path], create: bool = False):
        """
        Initialize temporary file.
        
        Args:
            filepath: Path to the file
            create: Create the file if it doesn't exist
        """
        self.filepath = Path(filepath)
        self.created = False
        
        if create and not self.filepath.exists():
            self.filepath.touch()
            self.created = True
    
    def __enter__(self):
        return self.filepath
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Silent deletion on exit
        try:
            if self.filepath.exists():
                self.filepath.unlink()
        except:
            pass  # Silently ignore any errors on cleanup


# ============================================
# DECORATOR FOR CLEANUP
# ============================================

def cleanup_files(*filepaths):
    """
    Decorator that ensures files are deleted after function execution.
    
    Args:
        *filepaths: Files to delete after function completes
    """
    def decorator(func):
        def wrapper(*args, **kwargs):
            try:
                result = func(*args, **kwargs)
                return result
            finally:
                # Always delete files, silently ignoring errors
                for filepath in filepaths:
                    delete_file_silent(filepath)
        return wrapper
    return decorator


# ============================================
# UTILITY FUNCTIONS
# ============================================

def delete_if_exists(filepath: Union[str, Path]) -> bool:
    """
    Delete a file only if it exists.
    
    Args:
        filepath: Path to the file
    
    Returns:
        True if file was deleted, False if it didn't exist
    """
    filepath = Path(filepath)
    if filepath.exists() and filepath.is_file():
        filepath.unlink()
        return True
    return False


def force_delete(filepath: Union[str, Path]) -> bool:
    """
    Try multiple methods to delete a file.
    
    Args:
        filepath: Path to the file
    
    Returns:
        True if file was deleted or didn't exist
    """
    filepath = Path(filepath)
    
    if not filepath.exists():
        return True
    
    # Try normal deletion
    try:
        filepath.unlink()
        return True
    except:
        pass
    
    # Try using os.remove
    try:
        os.remove(filepath)
        return True
    except:
        pass
    
    # On Windows, try to change attributes first
    if sys.platform == 'win32':
        try:
            import stat
            os.chmod(filepath, stat.S_IWRITE)
            os.remove(filepath)
            return True
        except:
            pass
    
    return False


def safe_cleanup(directory: Union[str, Path], pattern: str = "*") -> dict:
    """
    Clean up files matching a pattern in a directory.
    
    Args:
        directory: Directory to clean
        pattern: File pattern to match (e.g., "*.tmp")
    
    Returns:
        Statistics about cleaned files
    """
    directory = Path(directory)
    results = {'deleted': 0, 'errors': 0, 'files': []}
    
    if not directory.exists():
        return results
    
    for filepath in directory.glob(pattern):
        if filepath.is_file():
            if delete_if_exists(filepath):
                results['deleted'] += 1
                results['files'].append(str(filepath))
            else:
                results['errors'] += 1
    
    return results


# ============================================
# DEMO / TESTING
# ============================================

def create_test_files():
    """Create test files for demonstration."""
    test_files = []
    
    # Create some test files
    for i in range(3):
        filename = f"test_file_{i}.txt"
        with open(filename, 'w') as f:
            f.write(f"This is test file {i}")
        test_files.append(filename)
        print(f"✅ Created: {filename}")
    
    # Create a file that will be missing
    test_files.append("missing_file.txt")
    
    return test_files


def demo():
    """Demonstrate different deletion methods."""
    
    print("=" * 60)
    print("🗑️  SAFE FILE DELETION DEMO")
    print("=" * 60)
    
    # Create test files
    print("\n📁 Creating test files...")
    test_files = create_test_files()
    
    # Method 1: Basic silent deletion
    print("\n1️⃣  Basic silent deletion:")
    for filename in test_files:
        delete_file_silent(filename)
    
    # Method 2: Safe delete with options
    print("\n2️⃣  Safe delete with logging:")
    safe_delete("test_file_0.txt", silent=False, log=True)
    safe_delete("missing_file.txt", silent=False, log=True)
    
    # Method 3: Multiple file deletion
    print("\n3️⃣  Multiple file deletion:")
    files_to_delete = ["test_file_1.txt", "test_file_2.txt", "nonexistent.txt"]
    results = delete_multiple_files(files_to_delete, silent=False, summary=True)
    
    # Method 4: Pathlib version
    print("\n4️⃣  Pathlib version:")
    # Create a new test file
    with open("pathlib_test.txt", 'w') as f:
        f.write("Test for pathlib")
    
    if delete_with_pathlib("pathlib_test.txt"):
        print("✅ Deleted with pathlib")
    
    delete_with_pathlib("nonexistent.txt")  # Silent fail
    
    # Method 5: Context manager
    print("\n5️⃣  Context manager (auto-cleanup):")
    with TemporaryFile("temp_context.txt", create=True) as tmp:
        print(f"   Created: {tmp}")
        with open(tmp, 'w') as f:
            f.write("Temporary data")
        print(f"   File exists: {tmp.exists()}")
    print(f"   After context, file exists: {Path('temp_context.txt').exists()}")
    
    # Method 6: Decorator
    print("\n6️⃣  Decorator cleanup:")
    
    @cleanup_files("decorator_test.txt")
    def test_function():
        with open("decorator_test.txt", 'w') as f:
            f.write("Created in function")
        print("   File created in function")
        return "Function complete"
    
    result = test_function()
    print(f"   {result}")
    print(f"   File after function: {Path('decorator_test.txt').exists()}")
    
    # Method 7: Force delete
    print("\n7️⃣  Force delete:")
    # Create a read-only file on Unix
    ro_file = "readonly_test.txt"
    with open(ro_file, 'w') as f:
        f.write("Read-only test")
    
    # Make it read-only (Unix)
    if sys.platform != 'win32':
        os.chmod(ro_file, 0o444)
        print(f"   Made {ro_file} read-only")
    
    if force_delete(ro_file):
        print(f"   Force deleted: {ro_file}")
    else:
        print(f"   Could not delete: {ro_file}")
    
    # Method 8: Directory cleanup
    print("\n8️⃣  Directory cleanup:")
    os.makedirs("test_cleanup", exist_ok=True)
    for i in range(3):
        with open(f"test_cleanup/file_{i}.tmp", 'w') as f:
            f.write("temp")
    
    results = safe_cleanup("test_cleanup", "*.tmp")
    print(f"   Cleaned {results['deleted']} files")
    
    # Clean up test directory
    os.rmdir("test_cleanup")
    
    print("\n" + "=" * 60)
    print("✅ Demo complete")
    print("=" * 60)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Command line usage
        for filename in sys.argv[1:]:
            safe_delete(filename, silent=False)
    else:
        # Run demo
        demo()