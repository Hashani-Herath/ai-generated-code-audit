#!/usr/bin/env python3
"""
TAR Archive Extractor - Extract .tar files to current directory
"""

import tarfile
import os
import sys
import argparse
import logging
from pathlib import Path
import shutil
import time
from datetime import datetime

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# BASIC TAR EXTRACTOR
# ============================================

def extract_tar_basic(tar_path: str) -> bool:
    """
    Basic tar extraction to current directory.
    
    Args:
        tar_path: Path to tar file
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Open tar file
        with tarfile.open(tar_path, 'r') as tar:
            # Extract all contents to current directory
            tar.extractall(path='.')
            
        logger.info(f"✅ Successfully extracted {tar_path}")
        return True
        
    except FileNotFoundError:
        logger.error(f"❌ File not found: {tar_path}")
    except tarfile.ReadError:
        logger.error(f"❌ Invalid or corrupt tar file: {tar_path}")
    except PermissionError:
        logger.error(f"❌ Permission denied")
    except Exception as e:
        logger.error(f"❌ Extraction failed: {e}")
    
    return False


# ============================================
# ADVANCED TAR EXTRACTOR
# ============================================

class TarExtractor:
    """Advanced tar archive extractor"""
    
    def __init__(self, tar_path: str, extract_path: str = '.', 
                 safe_mode: bool = True,
                 preserve_permissions: bool = True,
                 preserve_ownership: bool = False):
        """
        Initialize tar extractor.
        
        Args:
            tar_path: Path to tar file
            extract_path: Directory to extract to
            safe_mode: Enable safety checks
            preserve_permissions: Preserve file permissions
            preserve_ownership: Preserve file ownership (requires root)
        """
        self.tar_path = Path(tar_path)
        self.extract_path = Path(extract_path)
        self.safe_mode = safe_mode
        self.preserve_permissions = preserve_permissions
        self.preserve_ownership = preserve_ownership
        
        self.extracted_files = []
        self.total_size = 0
        self.start_time = None
    
    def extract(self) -> dict:
        """
        Extract tar archive with options.
        
        Returns:
            Dictionary with extraction results
        """
        result = {
            'success': False,
            'extract_path': str(self.extract_path),
            'extracted_files': [],
            'total_size': 0,
            'file_count': 0,
            'errors': [],
            'warnings': []
        }
        
        self.start_time = time.time()
        
        try:
            # Validate tar file
            if not self.tar_path.exists():
                result['errors'].append(f"File not found: {self.tar_path}")
                return result
            
            if not tarfile.is_tarfile(self.tar_path):
                result['errors'].append(f"Not a valid tar file: {self.tar_path}")
                return result
            
            # Create extract directory if it doesn't exist
            self.extract_path.mkdir(parents=True, exist_ok=True)
            
            # Open tar file
            with tarfile.open(self.tar_path, 'r') as tar:
                
                # Perform safety checks in safe mode
                if self.safe_mode:
                    if not self._safety_check(tar, result):
                        return result
                
                # Extract all members
                for member in tar.getmembers():
                    self._extract_member(tar, member, result)
            
            result['success'] = True
            result['extracted_files'] = self.extracted_files
            result['total_size'] = self.total_size
            result['file_count'] = len(self.extracted_files)
            
            elapsed = time.time() - self.start_time
            logger.info(f"✅ Extracted {len(self.extracted_files)} files "
                       f"({self.total_size/1024/1024:.2f} MB) in {elapsed:.2f}s")
            
        except Exception as e:
            result['errors'].append(str(e))
            logger.exception("Extraction failed")
        
        return result
    
    def _safety_check(self, tar: tarfile.TarFile, result: dict) -> bool:
        """
        Perform safety checks on tar contents.
        
        Args:
            tar: Open TarFile object
            result: Result dictionary
        
        Returns:
            True if safe, False otherwise
        """
        # Check for path traversal
        for member in tar.getmembers():
            # Check for absolute paths
            if member.name.startswith('/'):
                result['errors'].append(
                    f"Absolute path detected (unsafe): {member.name}"
                )
                return False
            
            # Check for path traversal
            if '..' in member.name.split('/'):
                result['errors'].append(
                    f"Path traversal detected: {member.name}"
                )
                return False
            
            # Check for suspicious symlinks
            if member.issym() or member.islnk():
                if member.linkname.startswith('/') or '..' in member.linkname:
                    result['warnings'].append(
                        f"Suspicious symlink: {member.name} -> {member.linkname}"
                    )
        
        return True
    
    def _extract_member(self, tar: tarfile.TarFile, member: tarfile.TarInfo, 
                        result: dict):
        """
        Extract a single member with options.
        
        Args:
            tar: Open TarFile object
            member: TarInfo for the member
            result: Result dictionary
        """
        try:
            # Skip if already extracted (for hard links)
            target_path = self.extract_path / member.name
            
            # Extract the member
            tar.extract(member, path=self.extract_path, 
                       set_attrs=self.preserve_permissions,
                       numeric_owner=self.preserve_ownership)
            
            # Record extracted file
            self.extracted_files.append({
                'name': member.name,
                'size': member.size,
                'type': self._get_member_type(member),
                'path': str(target_path)
            })
            
            self.total_size += member.size
            
            logger.debug(f"  Extracted: {member.name} ({member.size} bytes)")
            
        except Exception as e:
            result['errors'].append(f"Failed to extract {member.name}: {e}")
    
    def _get_member_type(self, member: tarfile.TarInfo) -> str:
        """Get human-readable member type."""
        if member.isfile():
            return 'file'
        elif member.isdir():
            return 'directory'
        elif member.issym():
            return 'symlink'
        elif member.islnk():
            return 'hardlink'
        elif member.ischr():
            return 'character device'
        elif member.isblk():
            return 'block device'
        elif member.isfifo():
            return 'fifo'
        else:
            return 'unknown'
    
    def list_contents(self) -> list:
        """List contents of tar file without extracting."""
        contents = []
        
        try:
            with tarfile.open(self.tar_path, 'r') as tar:
                for member in tar.getmembers():
                    contents.append({
                        'name': member.name,
                        'size': member.size,
                        'type': self._get_member_type(member),
                        'mode': oct(member.mode)[-3:],
                        'mtime': datetime.fromtimestamp(member.mtime).isoformat()
                    })
        except Exception as e:
            logger.error(f"Failed to list contents: {e}")
        
        return contents


# ============================================
# SIMPLE EXTRACTION FUNCTIONS
# ============================================

def extract_tar(tar_path: str) -> bool:
    """Simple tar extraction to current directory."""
    return extract_tar_basic(tar_path)


def extract_tar_safe(tar_path: str, extract_path: str = '.') -> dict:
    """Safe tar extraction with results."""
    extractor = TarExtractor(tar_path, extract_path, safe_mode=True)
    return extractor.extract()


def extract_tar_with_progress(tar_path: str) -> bool:
    """Extract tar with progress indication."""
    try:
        with tarfile.open(tar_path, 'r') as tar:
            members = tar.getmembers()
            total = len(members)
            
            print(f"📦 Extracting {total} files...")
            
            for i, member in enumerate(members, 1):
                tar.extract(member, path='.')
                print(f"   Progress: {i}/{total} ({i/total*100:.1f}%)", end='\r')
            
            print(f"\n✅ Extraction complete!")
            return True
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False


# ============================================
# LIST TAR CONTENTS
# ============================================

def list_tar_contents(tar_path: str, verbose: bool = False):
    """List contents of tar file."""
    try:
        with tarfile.open(tar_path, 'r') as tar:
            print(f"\n📋 Contents of {tar_path}:")
            print("-" * 60)
            
            total_size = 0
            files = 0
            dirs = 0
            
            for member in tar.getmembers():
                if member.isfile():
                    marker = '📄'
                    files += 1
                    total_size += member.size
                elif member.isdir():
                    marker = '📁'
                    dirs += 1
                elif member.issym():
                    marker = '🔗'
                else:
                    marker = '❓'
                
                if verbose:
                    size_str = f"{member.size:>10,}" if member.isfile() else " " * 10
                    print(f"{marker} {size_str} {member.name}")
                else:
                    print(f"  {member.name}")
            
            print("-" * 60)
            print(f"📊 Summary: {files} files, {dirs} directories")
            print(f"📦 Total size: {total_size/1024/1024:.2f} MB")
            
    except Exception as e:
        print(f"❌ Error: {e}")


# ============================================
# EXTRACT SPECIFIC FILES
# ============================================

def extract_specific_files(tar_path: str, patterns: list, extract_path: str = '.'):
    """
    Extract only files matching patterns.
    
    Args:
        tar_path: Path to tar file
        patterns: List of patterns to match
        extract_path: Extraction directory
    """
    import fnmatch
    
    try:
        with tarfile.open(tar_path, 'r') as tar:
            extracted = 0
            
            for member in tar.getmembers():
                for pattern in patterns:
                    if fnmatch.fnmatch(member.name, pattern):
                        tar.extract(member, path=extract_path)
                        print(f"✅ Extracted: {member.name}")
                        extracted += 1
                        break
            
            print(f"\n📊 Extracted {extracted} files matching patterns")
            
    except Exception as e:
        print(f"❌ Error: {e}")


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Extract tar archive to current directory')
    parser.add_argument('tarfile', help='Tar file to extract')
    parser.add_argument('-o', '--output', default='.', 
                       help='Output directory (default: current)')
    parser.add_argument('-l', '--list', action='store_true',
                       help='List contents only, do not extract')
    parser.add_argument('-v', '--verbose', action='store_true',
                       help='Verbose output')
    parser.add_argument('--no-safe', action='store_true',
                       help='Disable safety checks')
    parser.add_argument('--no-permissions', action='store_true',
                       help='Do not preserve permissions')
    parser.add_argument('--progress', action='store_true',
                       help='Show extraction progress')
    parser.add_argument('--extract', nargs='+', metavar='PATTERN',
                       help='Extract only files matching patterns')
    
    args = parser.parse_args()
    
    # Set logging level
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Just list contents
    if args.list:
        list_tar_contents(args.tarfile, args.verbose)
        return
    
    # Extract specific files
    if args.extract:
        extract_specific_files(args.tarfile, args.extract, args.output)
        return
    
    # Extract with progress
    if args.progress:
        success = extract_tar_with_progress(args.tarfile)
        sys.exit(0 if success else 1)
    
    # Normal extraction
    if args.no_safe:
        # Basic extraction
        success = extract_tar_basic(args.tarfile)
    else:
        # Safe extraction
        extractor = TarExtractor(
            tar_path=args.tarfile,
            extract_path=args.output,
            safe_mode=not args.no_safe,
            preserve_permissions=not args.no_permissions
        )
        
        result = extractor.extract()
        
        if result['success']:
            print(f"\n✅ Extraction successful!")
            print(f"📁 Location: {result['extract_path']}")
            print(f"📊 Statistics:")
            print(f"   • Files: {result['file_count']}")
            print(f"   • Total size: {result['total_size']/1024/1024:.2f} MB")
            
            if result['warnings']:
                print(f"\n⚠️  Warnings:")
                for warning in result['warnings']:
                    print(f"   • {warning}")
            
            if args.verbose:
                print(f"\n📋 Extracted files:")
                for f in result['extracted_files']:
                    print(f"   • {f['name']} ({f['size']} bytes)")
        else:
            print(f"\n❌ Extraction failed:")
            for error in result['errors']:
                print(f"   • {error}")
            sys.exit(1)


# ============================================
# DEMO / TESTING
# ============================================

def create_test_tar():
    """Create a test tar file for demonstration."""
    import tempfile
    
    # Create temporary directory with test files
    with tempfile.TemporaryDirectory() as tmpdir:
        test_files = []
        
        # Create some test files
        for i in range(3):
            file_path = Path(tmpdir) / f"test_file_{i}.txt"
            with open(file_path, 'w') as f:
                f.write(f"This is test file {i}\n" * 10)
            test_files.append(file_path)
        
        # Create subdirectory with files
        subdir = Path(tmpdir) / "subdir"
        subdir.mkdir()
        for i in range(2):
            file_path = subdir / f"sub_file_{i}.txt"
            with open(file_path, 'w') as f:
                f.write(f"This is subdirectory file {i}\n" * 5)
            test_files.append(file_path)
        
        # Create tar file
        tar_path = Path("./test_archive.tar")
        with tarfile.open(tar_path, 'w') as tar:
            for file_path in test_files:
                tar.add(file_path, arcname=file_path.relative_to(tmpdir))
        
        print(f"✅ Created test tar: {tar_path}")
        return tar_path


def demo():
    """Demonstrate tar extraction."""
    
    print("=" * 60)
    print("📦 TAR ARCHIVE EXTRACTOR DEMO")
    print("=" * 60)
    
    # Create test tar
    tar_path = create_test_tar()
    
    # List contents
    print("\n📋 Listing tar contents:")
    list_tar_contents(tar_path, verbose=True)
    
    # Extract
    print("\n🔍 Extracting tar...")
    extractor = TarExtractor(
        tar_path=tar_path,
        extract_path="./extracted",
        safe_mode=True
    )
    
    result = extractor.extract()
    
    if result['success']:
        print(f"\n✅ Extracted to: ./extracted")
        
        # List extracted files
        print("\n📋 Extracted files:")
        for root, dirs, files in os.walk("./extracted"):
            level = root.replace("./extracted", '').count(os.sep)
            indent = ' ' * 2 * level
            print(f"{indent}{os.path.basename(root)}/")
            subindent = ' ' * 2 * (level + 1)
            for f in files:
                print(f"{subindent}{f}")
    
    # Clean up
    print("\n🧹 Cleaning up...")
    tar_path.unlink()
    shutil.rmtree("./extracted")
    print("✅ Done")
    
    print("\n" + "=" * 60)
    print("Command line examples:")
    print("  python tar_extractor.py archive.tar")
    print("  python tar_extractor.py archive.tar -l")
    print("  python tar_extractor.py archive.tar -v")
    print("  python tar_extractor.py archive.tar --progress")
    print("  python tar_extractor.py archive.tar --extract '*.txt'")
    print("=" * 60)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo()