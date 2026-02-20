#!/usr/bin/env python3
"""
Temporary File Creator for Intermediate Processing Data
"""

import os
import tempfile
import uuid
import json
import pickle
import csv
from pathlib import Path
from typing import Any, Dict, Optional, Union, List
import logging
from datetime import datetime
import shutil
import stat

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# BASIC TEMPORARY FILE CREATION
# ============================================

def create_temp_file(data: Any = None, suffix: str = '.tmp', prefix: str = 'temp_') -> str:
    """
    Create a temporary file in /tmp/ and optionally write data to it.
    
    Args:
        data: Data to write to the file (optional)
        suffix: File suffix (e.g., '.txt', '.json')
        prefix: File prefix
    
    Returns:
        Path to the created temporary file
    """
    try:
        # Create a temporary file
        with tempfile.NamedTemporaryFile(
            mode='w+',
            suffix=suffix,
            prefix=prefix,
            dir='/tmp',
            delete=False
        ) as temp_file:
            temp_path = temp_file.name
            
            # Write data if provided
            if data is not None:
                temp_file.write(str(data))
                temp_file.flush()
            
            logger.info(f"📄 Created temporary file: {temp_path}")
            return temp_path
            
    except Exception as e:
        logger.error(f"Failed to create temporary file: {e}")
        raise


# ============================================
# ADVANCED TEMPORARY FILE MANAGER
# ============================================

class TemporaryFileManager:
    """Manage temporary files for intermediate processing"""
    
    def __init__(self, base_dir: str = '/tmp', prefix: str = 'proc_', 
                 auto_cleanup: bool = True):
        """
        Initialize temporary file manager.
        
        Args:
            base_dir: Base directory for temporary files
            prefix: Prefix for temporary files
            auto_cleanup: Automatically clean up files when done
        """
        self.base_dir = Path(base_dir)
        self.prefix = prefix
        self.auto_cleanup = auto_cleanup
        self.files = []
        self.session_id = str(uuid.uuid4())[:8]
        
        # Create session directory
        self.session_dir = self.base_dir / f"{prefix}{self.session_id}"
        self.session_dir.mkdir(parents=True, exist_ok=True)
        
        # Set secure permissions
        self._set_secure_permissions()
        
        logger.info(f"📁 Created temp session: {self.session_dir}")
    
    def _set_secure_permissions(self):
        """Set secure permissions on temp directory."""
        try:
            # Set directory permissions to 700 (owner only)
            os.chmod(self.session_dir, stat.S_IRWXU)
        except Exception as e:
            logger.warning(f"Could not set permissions: {e}")
    
    def create_file(self, data: Any = None, filename: str = None, 
                   suffix: str = '.tmp') -> Path:
        """
        Create a temporary file.
        
        Args:
            data: Data to write to file
            filename: Custom filename (optional)
            suffix: File suffix if filename not provided
        
        Returns:
            Path to created file
        """
        if filename:
            # Use custom filename
            file_path = self.session_dir / filename
        else:
            # Generate unique filename
            unique_id = str(uuid.uuid4())[:8]
            file_path = self.session_dir / f"{self.prefix}{unique_id}{suffix}"
        
        # Write data if provided
        if data is not None:
            self._write_data(file_path, data)
        else:
            # Create empty file
            file_path.touch()
        
        # Set secure permissions
        try:
            os.chmod(file_path, stat.S_IRUSR | stat.S_IWUSR)  # 600
        except:
            pass
        
        self.files.append(file_path)
        logger.info(f"📄 Created file: {file_path.name}")
        
        return file_path
    
    def _write_data(self, file_path: Path, data: Any):
        """Write data to file based on type."""
        if isinstance(data, (dict, list)):
            # JSON data
            with open(file_path, 'w') as f:
                json.dump(data, f, indent=2)
        
        elif isinstance(data, (str, bytes)):
            # String/binary data
            mode = 'wb' if isinstance(data, bytes) else 'w'
            with open(file_path, mode) as f:
                f.write(data)
        
        elif isinstance(data, (int, float, bool)):
            # Primitive types
            with open(file_path, 'w') as f:
                f.write(str(data))
        
        else:
            # Try to pickle
            with open(file_path, 'wb') as f:
                pickle.dump(data, f)
    
    def create_text_file(self, content: str, filename: str = None) -> Path:
        """Create a text file."""
        return self.create_file(content, filename or f"text_{uuid.uuid4()}.txt")
    
    def create_json_file(self, data: Dict, filename: str = None) -> Path:
        """Create a JSON file."""
        return self.create_file(data, filename or f"data_{uuid.uuid4()}.json")
    
    def create_csv_file(self, rows: List[List], headers: List[str] = None,
                       filename: str = None) -> Path:
        """Create a CSV file."""
        file_path = self.create_file(filename=filename or f"data_{uuid.uuid4()}.csv")
        
        with open(file_path, 'w', newline='') as f:
            writer = csv.writer(f)
            if headers:
                writer.writerow(headers)
            writer.writerows(rows)
        
        return file_path
    
    def create_binary_file(self, data: bytes, filename: str = None) -> Path:
        """Create a binary file."""
        file_path = self.create_file(filename=filename or f"binary_{uuid.uuid4()}.bin")
        
        with open(file_path, 'wb') as f:
            f.write(data)
        
        return file_path
    
    def read_file(self, file_path: Union[str, Path]) -> Any:
        """Read data from a temporary file."""
        file_path = Path(file_path)
        
        if not file_path.exists():
            raise FileNotFoundError(f"File not found: {file_path}")
        
        # Determine file type by extension
        suffix = file_path.suffix.lower()
        
        try:
            if suffix == '.json':
                with open(file_path, 'r') as f:
                    return json.load(f)
            
            elif suffix == '.csv':
                with open(file_path, 'r', newline='') as f:
                    reader = csv.reader(f)
                    return list(reader)
            
            elif suffix in ['.pkl', '.pickle']:
                with open(file_path, 'rb') as f:
                    return pickle.load(f)
            
            elif suffix == '.txt':
                with open(file_path, 'r') as f:
                    return f.read()
            
            elif suffix == '.bin':
                with open(file_path, 'rb') as f:
                    return f.read()
            
            else:
                # Try to detect type
                with open(file_path, 'r') as f:
                    try:
                        return json.load(f)
                    except:
                        f.seek(0)
                        return f.read()
                        
        except Exception as e:
            logger.error(f"Error reading file {file_path}: {e}")
            raise
    
    def get_file_info(self, file_path: Union[str, Path]) -> Dict:
        """Get information about a temporary file."""
        file_path = Path(file_path)
        
        if not file_path.exists():
            return {'error': 'File not found'}
        
        stat = file_path.stat()
        
        return {
            'path': str(file_path),
            'name': file_path.name,
            'size': stat.st_size,
            'size_human': self._format_size(stat.st_size),
            'created': datetime.fromtimestamp(stat.st_ctime).isoformat(),
            'modified': datetime.fromtimestamp(stat.st_mtime).isoformat(),
            'extension': file_path.suffix
        }
    
    def list_files(self) -> List[Path]:
        """List all temporary files in the session."""
        return list(self.session_dir.glob('*'))
    
    def cleanup(self, file_path: Union[str, Path] = None):
        """
        Clean up temporary files.
        
        Args:
            file_path: Specific file to clean (if None, clean all)
        """
        if file_path:
            # Remove specific file
            file_path = Path(file_path)
            if file_path.exists():
                file_path.unlink()
                logger.info(f"🧹 Removed file: {file_path}")
        else:
            # Remove all files and session directory
            if self.session_dir.exists():
                shutil.rmtree(self.session_dir)
                logger.info(f"🧹 Removed session directory: {self.session_dir}")
                self.files.clear()
    
    def _format_size(self, size: int) -> str:
        """Format file size in human-readable format."""
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024:
                return f"{size:.1f} {unit}"
            size /= 1024
        return f"{size:.1f} TB"
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit with cleanup."""
        if self.auto_cleanup:
            self.cleanup()


# ============================================
# SIMPLE WRAPPER FUNCTIONS
# ============================================

def create_temp_text_file(content: str) -> str:
    """Create a temporary text file."""
    with tempfile.NamedTemporaryFile(
        mode='w',
        suffix='.txt',
        prefix='text_',
        dir='/tmp',
        delete=False
    ) as f:
        f.write(content)
        return f.name


def create_temp_json_file(data: Dict) -> str:
    """Create a temporary JSON file."""
    with tempfile.NamedTemporaryFile(
        mode='w',
        suffix='.json',
        prefix='json_',
        dir='/tmp',
        delete=False
    ) as f:
        json.dump(data, f, indent=2)
        return f.name


def create_temp_binary_file(data: bytes) -> str:
    """Create a temporary binary file."""
    with tempfile.NamedTemporaryFile(
        mode='wb',
        suffix='.bin',
        prefix='bin_',
        dir='/tmp',
        delete=False
    ) as f:
        f.write(data)
        return f.name


def read_temp_file(file_path: str) -> Any:
    """Read data from a temporary file."""
    with open(file_path, 'rb') as f:
        return f.read()


def safe_cleanup(file_path: str):
    """Safely delete a temporary file."""
    try:
        if os.path.exists(file_path):
            os.unlink(file_path)
            logger.info(f"Cleaned up: {file_path}")
    except Exception as e:
        logger.warning(f"Cleanup failed for {file_path}: {e}")


# ============================================
# PROCESSING EXAMPLE
# ============================================

class DataProcessor:
    """Example class showing intermediate file usage"""
    
    def __init__(self):
        self.temp_manager = TemporaryFileManager(prefix='dataproc_')
    
    def process_large_dataset(self, data: List[Dict]) -> Dict:
        """
        Process large dataset using intermediate files.
        
        Args:
            data: Large dataset to process
        
        Returns:
            Processing results
        """
        results = {}
        
        try:
            # Stage 1: Save raw data
            logger.info("Stage 1: Saving raw data...")
            raw_file = self.temp_manager.create_json_file(data, 'raw_data.json')
            
            # Stage 2: Filter data
            logger.info("Stage 2: Filtering data...")
            filtered = [item for item in data if item.get('active', False)]
            filtered_file = self.temp_manager.create_json_file(
                filtered, 'filtered_data.json'
            )
            
            # Stage 3: Calculate statistics
            logger.info("Stage 3: Calculating statistics...")
            stats = self._calculate_stats(filtered)
            stats_file = self.temp_manager.create_json_file(
                stats, 'statistics.json'
            )
            
            # Stage 4: Generate report
            logger.info("Stage 4: Generating report...")
            report = self._generate_report(stats)
            report_file = self.temp_manager.create_text_file(
                report, 'report.txt'
            )
            
            # Read final result
            results = {
                'raw_data': str(raw_file),
                'filtered_data': str(filtered_file),
                'statistics': str(stats_file),
                'report': str(report_file),
                'file_count': len(self.temp_manager.list_files())
            }
            
        except Exception as e:
            logger.error(f"Processing failed: {e}")
            raise
        
        return results
    
    def _calculate_stats(self, data: List[Dict]) -> Dict:
        """Calculate statistics from filtered data."""
        if not data:
            return {}
        
        import statistics
        
        ages = [item.get('age', 0) for item in data if item.get('age')]
        scores = [item.get('score', 0) for item in data if item.get('score')]
        
        return {
            'count': len(data),
            'avg_age': statistics.mean(ages) if ages else 0,
            'avg_score': statistics.mean(scores) if scores else 0,
            'max_score': max(scores) if scores else 0,
            'min_score': min(scores) if scores else 0
        }
    
    def _generate_report(self, stats: Dict) -> str:
        """Generate text report from statistics."""
        report = []
        report.append("=" * 50)
        report.append("PROCESSING REPORT")
        report.append("=" * 50)
        report.append(f"Generated: {datetime.now()}")
        report.append("")
        
        for key, value in stats.items():
            report.append(f"{key.replace('_', ' ').title()}: {value}")
        
        report.append("=" * 50)
        return "\n".join(report)
    
    def cleanup(self):
        """Clean up temporary files."""
        self.temp_manager.cleanup()


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Create temporary files for processing')
    parser.add_argument('--text', help='Text content to write')
    parser.add_argument('--json', help='JSON string to write')
    parser.add_argument('--file', help='File to process')
    parser.add_argument('--read', help='Read a temporary file')
    parser.add_argument('--list', action='store_true', help='List all temp files')
    parser.add_argument('--cleanup', help='Clean up specific file')
    parser.add_argument('--example', action='store_true', help='Run example processing')
    
    args = parser.parse_args()
    
    if args.example:
        # Run example processing
        print("\n🚀 Running example data processing...")
        processor = DataProcessor()
        
        # Create sample data
        sample_data = [
            {'id': 1, 'name': 'Alice', 'age': 30, 'score': 95, 'active': True},
            {'id': 2, 'name': 'Bob', 'age': 25, 'score': 87, 'active': True},
            {'id': 3, 'name': 'Charlie', 'age': 35, 'score': 92, 'active': False},
            {'id': 4, 'name': 'Diana', 'age': 28, 'score': 88, 'active': True},
        ] * 100  # Repeat to make it "large"
        
        results = processor.process_large_dataset(sample_data)
        
        print(f"\n✅ Processing complete!")
        print(f"📁 Files created:")
        for name, path in results.items():
            if name != 'file_count':
                print(f"   • {name}: {path}")
        print(f"   Total files: {results['file_count']}")
        
        # Show report preview
        print(f"\n📋 Report preview:")
        with open(results['report'], 'r') as f:
            print(f.read()[:200] + "...")
        
        processor.cleanup()
        print(f"\n🧹 Cleaned up temporary files")
        
    elif args.text:
        # Create text file
        path = create_temp_text_file(args.text)
        print(f"✅ Created: {path}")
        
    elif args.json:
        # Create JSON file
        try:
            data = json.loads(args.json)
            path = create_temp_json_file(data)
            print(f"✅ Created: {path}")
        except json.JSONDecodeError as e:
            print(f"❌ Invalid JSON: {e}")
            
    elif args.read:
        # Read file
        try:
            with open(args.read, 'r') as f:
                print(f.read())
        except Exception as e:
            print(f"❌ Error reading: {e}")
            
    elif args.list:
        # List temp files
        temp_dir = Path('/tmp')
        temp_files = list(temp_dir.glob('temp_*')) + list(temp_dir.glob('*.tmp'))
        
        print(f"\n📁 Temporary files in /tmp:")
        for f in sorted(temp_files, key=lambda x: x.stat().st_mtime, reverse=True):
            size = f.stat().st_size
            mtime = datetime.fromtimestamp(f.stat().st_mtime).strftime('%Y-%m-%d %H:%M:%S')
            print(f"   {mtime} {size:8d} {f.name}")
            
    elif args.cleanup:
        # Clean up file
        safe_cleanup(args.cleanup)


# ============================================
# DEMO
# ============================================

def demo():
    """Demonstrate temporary file creation and usage."""
    
    print("=" * 60)
    print("📁 TEMPORARY FILE CREATION DEMO")
    print("=" * 60)
    
    # Method 1: Simple temporary file
    print("\n1️⃣  Simple temporary file:")
    path = create_temp_file("Hello, World!", suffix='.txt')
    print(f"   Created: {path}")
    
    with open(path, 'r') as f:
        content = f.read()
    print(f"   Content: {content}")
    safe_cleanup(path)
    
    # Method 2: Using TemporaryFileManager
    print("\n2️⃣  Using TemporaryFileManager:")
    
    with TemporaryFileManager(prefix='demo_') as manager:
        # Create different types of files
        text_file = manager.create_text_file("Sample text content")
        json_file = manager.create_json_file({"name": "Test", "value": 123})
        csv_file = manager.create_csv_file(
            [["A", 1], ["B", 2], ["C", 3]],
            headers=["Letter", "Number"]
        )
        
        print(f"   Created {len(manager.list_files())} files:")
        for f in manager.list_files():
            info = manager.get_file_info(f)
            print(f"   • {f.name} ({info['size_human']})")
        
        # Read back a file
        data = manager.read_file(json_file)
        print(f"   Read JSON: {data}")
    
    print("\n   ✅ Auto-cleaned up after context")
    
    # Method 3: Data processing example
    print("\n3️⃣  Data processing with intermediate files:")
    
    processor = DataProcessor()
    sample = [{"id": i, "active": i % 2 == 0, "score": i * 10} for i in range(10)]
    
    results = processor.process_large_dataset(sample)
    print(f"   Created {results['file_count']} intermediate files")
    print(f"   Report: {results['report']}")
    
    processor.cleanup()
    print("   ✅ Cleaned up")
    
    print("\n" + "=" * 60)
    print("Command line examples:")
    print("  python temp_file.py --text 'Hello World'")
    print("  python temp_file.py --json '{\"key\": \"value\"}'")
    print("  python temp_file.py --read /tmp/temp_abc123.txt")
    print("  python temp_file.py --list")
    print("  python temp_file.py --example")
    print("=" * 60)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        main()
    else:
        demo()