#!/usr/bin/env python3
"""
File Reader Without 'with' Statement
"""

import sys
import os


# ============================================
# BASIC FILE READER (NO 'WITH')
# ============================================

def print_file_content(filename):
    """
    Open a file for reading and print its content.
    Does NOT use 'with' statement.
    
    Args:
        filename: Path to the file to read
    
    Returns:
        bool: True if successful, False otherwise
    """
    # Initialize file handle
    file = None
    
    try:
        # Open the file
        file = open(filename, 'r', encoding='utf-8')
        
        # Read and print the entire content
        content = file.read()
        print(content)
        
        return True
        
    except FileNotFoundError:
        print(f"❌ Error: File '{filename}' not found")
        return False
        
    except PermissionError:
        print(f"❌ Error: Permission denied to read '{filename}'")
        return False
        
    except UnicodeDecodeError:
        print(f"❌ Error: File '{filename}' is not a valid text file")
        return False
        
    except Exception as e:
        print(f"❌ Error reading file: {e}")
        return False
        
    finally:
        # Always close the file if it was opened
        if file:
            file.close()
            print(f"📁 File '{filename}' closed")


# ============================================
# ADVANCED FILE READER WITH OPTIONS
# ============================================

class FileReader:
    """File reader with multiple options, no 'with' statement used"""
    
    def __init__(self, filename, encoding='utf-8', mode='r'):
        """
        Initialize file reader.
        
        Args:
            filename: Path to file
            encoding: File encoding
            mode: Open mode ('r', 'rb', etc.)
        """
        self.filename = filename
        self.encoding = encoding
        self.mode = mode
        self.file = None
        self.is_open = False
    
    def open(self):
        """Open the file."""
        try:
            self.file = open(self.filename, self.mode, encoding=self.encoding)
            self.is_open = True
            print(f"📂 Opened: {self.filename}")
            return True
        except Exception as e:
            print(f"❌ Failed to open {self.filename}: {e}")
            return False
    
    def close(self):
        """Close the file if open."""
        if self.file:
            self.file.close()
            self.is_open = False
            print(f"📂 Closed: {self.filename}")
    
    def print_all(self):
        """Print entire file content."""
        if not self.is_open and not self.open():
            return False
        
        try:
            # Reset position to beginning
            self.file.seek(0)
            
            # Read and print
            content = self.file.read()
            print(content)
            return True
            
        except Exception as e:
            print(f"❌ Error reading file: {e}")
            return False
    
    def print_line_by_line(self, max_lines=None):
        """Print file line by line."""
        if not self.is_open and not self.open():
            return False
        
        try:
            self.file.seek(0)
            line_count = 0
            
            for line in self.file:
                print(line.rstrip())  # rstrip to avoid double newlines
                line_count += 1
                
                if max_lines and line_count >= max_lines:
                    print(f"\n... (stopped after {max_lines} lines)")
                    break
            
            print(f"\n📊 Printed {line_count} lines")
            return True
            
        except Exception as e:
            print(f"❌ Error reading file: {e}")
            return False
    
    def print_with_numbers(self):
        """Print file with line numbers."""
        if not self.is_open and not self.open():
            return False
        
        try:
            self.file.seek(0)
            for i, line in enumerate(self.file, 1):
                print(f"{i:4d} | {line.rstrip()}")
            return True
            
        except Exception as e:
            print(f"❌ Error reading file: {e}")
            return False
    
    def print_chunks(self, chunk_size=1024):
        """Print file in chunks (useful for large files)."""
        if not self.is_open and not self.open():
            return False
        
        try:
            self.file.seek(0)
            chunk_num = 1
            
            while True:
                chunk = self.file.read(chunk_size)
                if not chunk:
                    break
                
                print(f"\n--- Chunk {chunk_num} ---")
                print(chunk)
                chunk_num += 1
            
            print(f"\n📊 Read {chunk_num-1} chunks")
            return True
            
        except Exception as e:
            print(f"❌ Error reading file: {e}")
            return False
    
    def __del__(self):
        """Destructor to ensure file is closed."""
        self.close()


# ============================================
# SIMPLE FUNCTIONS (NO CLASS)
# ============================================

def print_file_simple(filename):
    """Simplest version - open, read, print, close."""
    file = None
    try:
        file = open(filename, 'r')
        print(file.read())
    except Exception as e:
        print(f"Error: {e}")
    finally:
        if file:
            file.close()


def print_file_with_encoding(filename, encoding='utf-8'):
    """Print file with specified encoding."""
    file = None
    try:
        file = open(filename, 'r', encoding=encoding)
        for line in file:
            print(line, end='')
    except UnicodeDecodeError:
        print(f"❌ Cannot decode with {encoding}. Try different encoding.")
    finally:
        if file:
            file.close()


def print_file_binary(filename):
    """Print file as binary (hex dump)."""
    file = None
    try:
        file = open(filename, 'rb')
        bytes_read = file.read(16)  # Read 16 bytes at a time
        offset = 0
        
        while bytes_read:
            # Print hex representation
            hex_str = ' '.join(f'{b:02x}' for b in bytes_read)
            hex_str = hex_str.ljust(48)  # Pad for alignment
            
            # Print ASCII representation
            ascii_str = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in bytes_read)
            
            print(f'{offset:08x}: {hex_str}  {ascii_str}')
            
            offset += len(bytes_read)
            bytes_read = file.read(16)
            
    except Exception as e:
        print(f"❌ Error: {e}")
    finally:
        if file:
            file.close()


def print_file_with_seek(filename, start=0, size=None):
    """Print portion of a file using seek."""
    file = None
    try:
        file = open(filename, 'rb')
        
        # Move to starting position
        file.seek(start)
        
        # Read specified size or rest of file
        if size:
            content = file.read(size)
        else:
            content = file.read()
        
        # Try to decode as text
        try:
            print(content.decode('utf-8'))
        except UnicodeDecodeError:
            print(f"[Binary data, {len(content)} bytes]")
            
    except Exception as e:
        print(f"❌ Error: {e}")
    finally:
        if file:
            file.close()


def print_multiple_files(filenames):
    """Print multiple files sequentially."""
    for filename in filenames:
        print(f"\n{'='*50}")
        print(f"File: {filename}")
        print('='*50)
        print_file_simple(filename)


# ============================================
# ERROR HANDLING FUNCTIONS
# ============================================

def safe_file_print(filename, encoding='utf-8'):
    """Safely print file with comprehensive error handling."""
    file = None
    result = {'success': False, 'error': None, 'line_count': 0}
    
    try:
        # Check if file exists
        if not os.path.exists(filename):
            result['error'] = f"File '{filename}' does not exist"
            return result
        
        # Check if it's a file (not a directory)
        if not os.path.isfile(filename):
            result['error'] = f"'{filename}' is not a file"
            return result
        
        # Check file size (optional)
        file_size = os.path.getsize(filename)
        if file_size > 10 * 1024 * 1024:  # 10MB
            print(f"⚠️  Warning: File is large ({file_size/1024/1024:.1f} MB)")
            response = input("Continue? (y/n): ")
            if response.lower() != 'y':
                result['error'] = "Cancelled by user"
                return result
        
        # Open and read file
        file = open(filename, 'r', encoding=encoding)
        
        # Read and print line by line
        line_count = 0
        for line in file:
            print(line, end='')
            line_count += 1
        
        result['success'] = True
        result['line_count'] = line_count
        
    except UnicodeDecodeError:
        result['error'] = f"Cannot decode file with {encoding} encoding"
    except PermissionError:
        result['error'] = f"Permission denied to read '{filename}'"
    except Exception as e:
        result['error'] = str(e)
    finally:
        if file:
            file.close()
    
    return result


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command line interface for file printing."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Print file content (no "with" statement)')
    parser.add_argument('file', help='File to print')
    parser.add_argument('-e', '--encoding', default='utf-8', 
                       help='File encoding (default: utf-8)')
    parser.add_argument('-n', '--line-numbers', action='store_true',
                       help='Show line numbers')
    parser.add_argument('-l', '--lines', type=int, 
                       help='Print only first N lines')
    parser.add_argument('-b', '--binary', action='store_true',
                       help='Print as binary/hex dump')
    parser.add_argument('-c', '--chunk-size', type=int, default=1024,
                       help='Chunk size in bytes (for chunked reading)')
    
    args = parser.parse_args()
    
    if args.binary:
        print_file_binary(args.file)
    elif args.line_numbers:
        reader = FileReader(args.file, args.encoding)
        reader.print_with_numbers()
        reader.close()
    elif args.lines:
        reader = FileReader(args.file, args.encoding)
        reader.print_line_by_line(max_lines=args.lines)
        reader.close()
    else:
        result = safe_file_print(args.file, args.encoding)
        if not result['success']:
            print(f"❌ Error: {result['error']}")
            sys.exit(1)
        else:
            print(f"\n📊 Printed {result['line_count']} lines")


# ============================================
# DEMO
# ============================================

def demo():
    """Demonstrate different file reading methods."""
    
    # Create a test file
    test_filename = 'test_demo.txt'
    test_content = """Line 1: Hello, World!
Line 2: This is a test file.
Line 3: It has multiple lines.
Line 4: Last line of the file."""

    file = None
    try:
        # Create test file
        file = open(test_filename, 'w')
        file.write(test_content)
        print(f"✅ Created test file: {test_filename}")
    finally:
        if file:
            file.close()
    
    print("\n" + "=" * 50)
    print("📄 FILE READING DEMO (No 'with')")
    print("=" * 50)
    
    # Method 1: Basic read
    print("\n1️⃣  Basic file read:")
    print_file_simple(test_filename)
    
    # Method 2: Line by line with numbers
    print("\n2️⃣  Line by line with numbers:")
    reader = FileReader(test_filename)
    reader.print_with_numbers()
    reader.close()
    
    # Method 3: First 2 lines only
    print("\n3️⃣  First 2 lines only:")
    reader = FileReader(test_filename)
    reader.print_line_by_line(max_lines=2)
    reader.close()
    
    # Method 4: Binary/hex dump
    print("\n4️⃣  Binary/hex dump:")
    print_file_binary(test_filename)
    
    # Clean up
    os.remove(test_filename)
    print(f"\n🧹 Cleaned up: {test_filename}")
    
    print("\n" + "=" * 50)
    print("Command line examples:")
    print(f"  python {sys.argv[0]} file.txt")
    print(f"  python {sys.argv[0]} file.txt -n")
    print(f"  python {sys.argv[0]} file.txt -l 10")
    print(f"  python {sys.argv[0]} file.txt -b")
    print("=" * 50)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo()