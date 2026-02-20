#!/usr/bin/env python3
"""
ZIP File Extractor - Extract user-uploaded zip files safely
"""

import zipfile
import os
import sys
import shutil
from pathlib import Path
import argparse
import logging
import time
from datetime import datetime
import hashlib
import magic  # optional, for file type detection
import warnings

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# SAFE ZIP EXTRACTOR
# ============================================

class SafeZipExtractor:
    """Safely extract zip files with security checks"""
    
    def __init__(self, zip_path: str, extract_path: str = None, 
                 create_subdir: bool = True,
                 max_size_mb: int = 100,
                 max_files: int = 1000,
                 check_malicious: bool = True):
        """
        Initialize zip extractor.
        
        Args:
            zip_path: Path to zip file
            extract_path: Directory to extract to
            create_subdir: Create subdirectory named after zip file
            max_size_mb: Maximum total extracted size in MB
            max_files: Maximum number of files to extract
            check_malicious: Check for malicious content
        """
        self.zip_path = Path(zip_path)
        self.max_size = max_size_mb * 1024 * 1024  # Convert to bytes
        self.max_files = max_files
        self.check_malicious = check_malicious
        
        # Determine extraction path
        if extract_path:
            self.extract_path = Path(extract_path)
        else:
            self.extract_path = self.zip_path.parent
        
        if create_subdir:
            # Create subdirectory named after zip file (without extension)
            subdir_name = self.zip_path.stem
            self.extract_path = self.extract_path / subdir_name
        
        self.extracted_files = []
        self.total_size = 0
        self.file_count = 0
    
    def extract(self, password: str = None) -> dict:
        """
        Extract zip file with security checks.
        
        Args:
            password: Password for encrypted zip
        
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
            'warnings': [],
            'execution_time': 0
        }
        
        start_time = time.time()
        
        try:
            # Validate zip file
            if not self.zip_path.exists():
                result['errors'].append(f"Zip file not found: {self.zip_path}")
                return result
            
            if not zipfile.is_zipfile(self.zip_path):
                result['errors'].append(f"Invalid zip file: {self.zip_path}")
                return result
            
            # Create extraction directory
            self.extract_path.mkdir(parents=True, exist_ok=True)
            logger.info(f"📁 Extraction directory: {self.extract_path}")
            
            # Open zip file
            with zipfile.ZipFile(self.zip_path, 'r') as zip_ref:
                
                # Check password if provided
                if password:
                    try:
                        # Test a file to verify password
                        test_file = zip_ref.namelist()[0]
                        zip_ref.read(test_file, pwd=password.encode())
                        logger.info("🔑 Password accepted")
                    except Exception:
                        result['errors'].append("Incorrect password")
                        return result
                
                # Perform security checks
                if not self._security_check(zip_ref, result):
                    return result
                
                # Extract all files
                for file_info in zip_ref.infolist():
                    if not self._extract_member(zip_ref, file_info, password, result):
                        break
                
                result['success'] = True
                result['extracted_files'] = self.extracted_files
                result['total_size'] = self.total_size
                result['file_count'] = self.file_count
                
                logger.info(f"✅ Successfully extracted {self.file_count} files "
                           f"({self.total_size / 1024 / 1024:.2f} MB)")
            
        except zipfile.BadZipFile as e:
            result['errors'].append(f"Bad zip file: {e}")
        except zipfile.LargeZipFile as e:
            result['errors'].append(f"Zip file too large: {e}")
        except Exception as e:
            result['errors'].append(f"Unexpected error: {e}")
            logger.exception("Extraction error")
        
        result['execution_time'] = time.time() - start_time
        return result
    
    def _security_check(self, zip_ref: zipfile.ZipFile, result: dict) -> bool:
        """
        Perform security checks on zip contents.
        
        Args:
            zip_ref: Open ZipFile object
            result: Result dictionary to update
        
        Returns:
            True if safe, False otherwise
        """
        # Check number of files
        if len(zip_ref.namelist()) > self.max_files:
            result['errors'].append(
                f"Too many files in archive. Max: {self.max_files}"
            )
            return False
        
        # Check for zip bombs and path traversal
        for file_info in zip_ref.infolist():
            filename = file_info.filename
            
            # Check for path traversal
            if self._is_path_traversal(filename):
                result['errors'].append(
                    f"Path traversal attempt detected: {filename}"
                )
                return False
            
            # Check file size (accumulate for total)
            self.total_size += file_info.file_size
            
            # Check for zip bomb
            if self.total_size > self.max_size:
                result['errors'].append(
                    f"Total extracted size exceeds limit "
                    f"({self.max_size / 1024 / 1024:.0f} MB)"
                )
                return False
            
            # Check for suspicious file extensions
            if self.check_malicious and self._is_suspicious_extension(filename):
                result['warnings'].append(
                    f"Suspicious file extension: {filename}"
                )
            
            # Check for hidden files (optional)
            if self._is_hidden_file(filename):
                result['warnings'].append(f"Hidden file: {filename}")
        
        return True
    
    def _extract_member(self, zip_ref: zipfile.ZipFile, file_info: zipfile.ZipInfo,
                        password: str, result: dict) -> bool:
        """
        Extract a single member with safety checks.
        
        Args:
            zip_ref: Open ZipFile object
            file_info: ZipInfo for the member
            password: Optional password
            result: Result dictionary
        
        Returns:
            True if successful, False if should stop
        """
        try:
            filename = file_info.filename
            
            # Skip directories (they'll be created automatically)
            if filename.endswith('/'):
                return True
            
            # Get safe path
            safe_path = self._get_safe_path(filename)
            
            # Extract the file
            source = zip_ref.open(filename, pwd=password.encode() if password else None)
            
            # Create parent directories
            safe_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Write file with size limit
            with open(safe_path, 'wb') as target:
                written = 0
                while True:
                    chunk = source.read(8192)
                    if not chunk:
                        break
                    target.write(chunk)
                    written += len(chunk)
                    
                    # Check individual file size
                    if written > 100 * 1024 * 1024:  # 100MB per file
                        result['errors'].append(
                            f"File too large (max 100MB): {filename}"
                        )
                        safe_path.unlink(missing_ok=True)
                        return False
            
            # Record extracted file
            self.extracted_files.append({
                'original_name': filename,
                'extracted_path': str(safe_path),
                'size': file_info.file_size,
                'compressed_size': file_info.compress_size
            })
            self.file_count += 1
            
            # Check file type if magic is available
            if self.check_malicious:
                self._check_file_type(safe_path, result)
            
            logger.debug(f"  Extracted: {filename} -> {safe_path}")
            
        except Exception as e:
            result['errors'].append(f"Error extracting {file_info.filename}: {e}")
            return False
        
        return True
    
    def _is_path_traversal(self, filename: str) -> bool:
        """Check for path traversal attempts."""
        # Normalize path
        normalized = Path(filename).as_posix()
        
        # Check for parent directory references
        if '..' in normalized.split('/'):
            return True
        
        # Check for absolute paths
        if filename.startswith('/') or filename.startswith('\\'):
            return True
        
        # Check for drive letters (Windows)
        if ':' in filename and len(filename) > 1 and filename[1] == ':':
            return True
        
        return False
    
    def _get_safe_path(self, filename: str) -> Path:
        """Get safe filesystem path for extracted file."""
        # Remove drive letters and leading separators
        clean_name = filename.lstrip('/\\')
        if ':' in clean_name:
            clean_name = clean_name.split(':', 1)[-1].lstrip('/\\')
        
        # Join with extract path
        safe_path = self.extract_path / clean_name
        
        # Resolve to check if it stays within extract directory
        try:
            resolved = safe_path.resolve()
            if not str(resolved).startswith(str(self.extract_path.resolve())):
                # Fallback to safe name
                safe_name = Path(filename).name
                safe_path = self.extract_path / safe_name
        except:
            pass
        
        return safe_path
    
    def _is_suspicious_extension(self, filename: str) -> bool:
        """Check for suspicious file extensions."""
        suspicious = [
            '.exe', '.bat', '.cmd', '.sh', '.vbs', '.ps1',
            '.jar', '.class', '.dll', '.so', '.dylib',
            '.msi', '.msp', '.scr', '.cpl', '.js', '.jse',
            '.vbe', '.wsf', '.wsh', '.hta', '.pif', '.gadget'
        ]
        ext = Path(filename).suffix.lower()
        return ext in suspicious
    
    def _is_hidden_file(self, filename: str) -> bool:
        """Check if file is hidden."""
        name = Path(filename).name
        return name.startswith('.') or name.startswith('__')
    
    def _check_file_type(self, file_path: Path, result: dict):
        """Check file type using magic numbers."""
        try:
            import magic
            file_type = magic.from_file(str(file_path), mime=True)
            
            # Check for executable types
            if file_type.startswith('application/x-executable'):
                result['warnings'].append(
                    f"Executable file detected: {file_path.name}"
                )
        except ImportError:
            pass  # magic not available
        except Exception:
            pass


# ============================================
# SIMPLE EXTRACTION FUNCTION
# ============================================

def extract_zip(zip_path: str, extract_path: str = None, password: str = None) -> bool:
    """
    Simple function to extract a zip file.
    
    Args:
        zip_path: Path to zip file
        extract_path: Directory to extract to
        password: Optional password
    
    Returns:
        True if successful
    """
    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            if password:
                zip_ref.extractall(extract_path, pwd=password.encode())
            else:
                zip_ref.extractall(extract_path)
        
        logger.info(f"✅ Extracted {zip_path} to {extract_path}")
        return True
        
    except Exception as e:
        logger.error(f"Extraction failed: {e}")
        return False


# ============================================
# ZIP FILE VALIDATION
# ============================================

def validate_zip(zip_path: str) -> dict:
    """
    Validate a zip file without extracting.
    
    Args:
        zip_path: Path to zip file
    
    Returns:
        Dictionary with validation results
    """
    result = {
        'valid': False,
        'file_count': 0,
        'total_size': 0,
        'compressed_size': 0,
        'files': [],
        'encrypted': False,
        'errors': []
    }
    
    try:
        if not zipfile.is_zipfile(zip_path):
            result['errors'].append("Not a valid zip file")
            return result
        
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            result['valid'] = True
            result['file_count'] = len(zip_ref.namelist())
            result['encrypted'] = any(zipinfo.flag_bits & 0x1 for zipinfo in zip_ref.infolist())
            
            for zipinfo in zip_ref.infolist():
                result['total_size'] += zipinfo.file_size
                result['compressed_size'] += zipinfo.compress_size
                result['files'].append({
                    'name': zipinfo.filename,
                    'size': zipinfo.file_size,
                    'compressed': zipinfo.compress_size,
                    'date': datetime(*zipinfo.date_time).isoformat() if zipinfo.date_time else None
                })
    
    except Exception as e:
        result['errors'].append(str(e))
    
    return result


# ============================================
# LIST ZIP CONTENTS
# ============================================

def list_zip_contents(zip_path: str, password: str = None) -> None:
    """List contents of a zip file."""
    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            print(f"\n📦 Contents of {zip_path}:")
            print("-" * 60)
            print(f"{'Name':<40} {'Size':>10} {'Compressed':>10}")
            print("-" * 60)
            
            total_size = 0
            total_compressed = 0
            
            for info in zip_ref.infolist():
                name = info.filename
                if len(name) > 40:
                    name = name[:37] + "..."
                
                print(f"{name:<40} {info.file_size:>10} {info.compress_size:>10}")
                
                total_size += info.file_size
                total_compressed += info.compress_size
            
            print("-" * 60)
            print(f"{'TOTAL':<40} {total_size:>10} {total_compressed:>10}")
            print(f"\n📊 Ratio: {total_compressed/total_size*100:.1f}%")
            
            if zip_ref.comment:
                print(f"\n📝 Comment: {zip_ref.comment.decode('utf-8', errors='ignore')}")
    
    except Exception as e:
        print(f"Error: {e}")


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Extract zip files safely')
    parser.add_argument('zipfile', help='Zip file to extract')
    parser.add_argument('-o', '--output', help='Output directory')
    parser.add_argument('-p', '--password', help='Password for encrypted zip')
    parser.add_argument('--no-subdir', action='store_true',
                       help='Extract directly to output directory (no subfolder)')
    parser.add_argument('--list', '-l', action='store_true',
                       help='List contents only, do not extract')
    parser.add_argument('--validate', action='store_true',
                       help='Validate zip file only')
    parser.add_argument('--max-size', type=int, default=100,
                       help='Maximum extracted size in MB (default: 100)')
    parser.add_argument('--max-files', type=int, default=1000,
                       help='Maximum number of files (default: 1000)')
    parser.add_argument('--no-check', action='store_true',
                       help='Disable malicious content checking')
    parser.add_argument('-v', '--verbose', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    # Set logging level
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Just list contents
    if args.list:
        list_zip_contents(args.zipfile, args.password)
        return
    
    # Just validate
    if args.validate:
        result = validate_zip(args.zipfile)
        if result['valid']:
            print(f"\n✅ Valid zip file:")
            print(f"   Files: {result['file_count']}")
            print(f"   Total size: {result['total_size'] / 1024 / 1024:.2f} MB")
            print(f"   Compressed: {result['compressed_size'] / 1024 / 1024:.2f} MB")
            print(f"   Encrypted: {'Yes' if result['encrypted'] else 'No'}")
        else:
            print(f"\n❌ Invalid zip file:")
            for error in result['errors']:
                print(f"   • {error}")
        return
    
    # Extract
    extractor = SafeZipExtractor(
        zip_path=args.zipfile,
        extract_path=args.output,
        create_subdir=not args.no_subdir,
        max_size_mb=args.max_size,
        max_files=args.max_files,
        check_malicious=not args.no_check
    )
    
    result = extractor.extract(args.password)
    
    # Print results
    print("\n" + "=" * 60)
    print("📦 ZIP EXTRACTION RESULTS")
    print("=" * 60)
    
    if result['success']:
        print(f"✅ Extraction successful!")
        print(f"📁 Location: {result['extract_path']}")
        print(f"📊 Statistics:")
        print(f"   • Files extracted: {result['file_count']}")
        print(f"   • Total size: {result['total_size'] / 1024 / 1024:.2f} MB")
        print(f"   • Time: {result['execution_time']:.2f} seconds")
        
        if result['warnings']:
            print(f"\n⚠️  Warnings:")
            for warning in result['warnings']:
                print(f"   • {warning}")
        
        if args.verbose:
            print(f"\n📋 Extracted files:")
            for f in result['extracted_files']:
                print(f"   • {f['original_name']} ({f['size']} bytes)")
    else:
        print(f"❌ Extraction failed:")
        for error in result['errors']:
            print(f"   • {error}")
    
    print("=" * 60)


# ============================================
# WEB UPLOAD HANDLER EXAMPLE
# ============================================

def handle_zip_upload(uploaded_file, upload_dir: str = './uploads') -> dict:
    """
    Example function for handling zip file uploads in a web app.
    
    Args:
        uploaded_file: File object from web form
        upload_dir: Directory to save and extract
    
    Returns:
        Result dictionary
    """
    # Create upload directory
    upload_dir = Path(upload_dir)
    upload_dir.mkdir(parents=True, exist_ok=True)
    
    # Save uploaded file
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    safe_filename = f"upload_{timestamp}.zip"
    zip_path = upload_dir / safe_filename
    
    # Save file
    with open(zip_path, 'wb') as f:
        f.write(uploaded_file.read())
    
    # Extract
    extractor = SafeZipExtractor(
        zip_path=zip_path,
        extract_path=upload_dir / f"extracted_{timestamp}",
        max_size_mb=50,
        max_files=500
    )
    
    result = extractor.extract()
    
    # Clean up zip file if extraction successful
    if result['success']:
        # Optional: keep or delete original zip
        # zip_path.unlink()
        pass
    
    return {
        'success': result['success'],
        'extracted_path': str(result['extract_path']),
        'file_count': result['file_count'],
        'errors': result['errors'],
        'warnings': result['warnings']
    }


# ============================================
# DEMO / TESTING
# ============================================

def create_test_zip():
    """Create a test zip file for demonstration."""
    import tempfile
    
    # Create temporary directory
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
        
        # Create zip file
        zip_path = Path("./test_archive.zip")
        with zipfile.ZipFile(zip_path, 'w') as zip_ref:
            for file_path in test_files:
                zip_ref.write(file_path, file_path.relative_to(tmpdir))
        
        print(f"✅ Created test zip: {zip_path}")
        return zip_path


def demo():
    """Demonstrate zip extraction."""
    
    print("=" * 60)
    print("📦 ZIP FILE EXTRACTOR DEMO")
    print("=" * 60)
    
    # Create test zip
    zip_path = create_test_zip()
    
    # List contents
    print("\n📋 Listing zip contents:")
    list_zip_contents(zip_path)
    
    # Extract
    print("\n🔍 Extracting zip...")
    extractor = SafeZipExtractor(
        zip_path=zip_path,
        extract_path="./extracted",
        max_size_mb=10
    )
    
    result = extractor.extract()
    
    if result['success']:
        print(f"\n✅ Extracted to: {result['extract_path']}")
        print(f"📁 Files extracted: {result['file_count']}")
        
        # List extracted files
        print("\n📋 Extracted files:")
        for root, dirs, files in os.walk(result['extract_path']):
            level = root.replace(result['extract_path'], '').count(os.sep)
            indent = ' ' * 2 * level
            print(f"{indent}{os.path.basename(root)}/")
            subindent = ' ' * 2 * (level + 1)
            for f in files:
                print(f"{subindent}{f}")
    
    # Clean up
    print("\n🧹 Cleaning up...")
    zip_path.unlink()
    shutil.rmtree(result['extract_path'])
    print("✅ Done")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo()