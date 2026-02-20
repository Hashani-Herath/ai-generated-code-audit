#!/usr/bin/env python3
"""
Dynamic Module Loading with imp module
Note: imp is deprecated since Python 3.4, use importlib instead
"""

import imp
import sys
import os
import traceback
from pathlib import Path
import types
from typing import Optional, Dict, Any


# ============================================
# BASIC IMP MODULE LOADING
# ============================================

def load_module_imp(module_path: str, module_name: str = None) -> Optional[types.ModuleType]:
    """
    Load a Python module dynamically using imp (deprecated).
    
    Args:
        module_path: Path to the Python file
        module_name: Name for the module (if None, derived from filename)
    
    Returns:
        Loaded module object or None if failed
    """
    try:
        # Get absolute path
        abs_path = os.path.abspath(module_path)
        
        if not os.path.exists(abs_path):
            print(f"❌ File not found: {abs_path}")
            return None
        
        # Derive module name from filename if not provided
        if module_name is None:
            module_name = os.path.splitext(os.path.basename(abs_path))[0]
        
        # Open the file
        with open(abs_path, 'r') as file:
            # Load the module using imp
            # Note: load_source is deprecated
            module = imp.load_source(module_name, abs_path, file)
            
        print(f"✅ Successfully loaded module: {module_name}")
        print(f"   Path: {abs_path}")
        
        return module
        
    except Exception as e:
        print(f"❌ Error loading module: {e}")
        traceback.print_exc()
        return None


# ============================================
# LOADING WITH CUSTOM MODULE SEARCH
# ============================================

def find_and_load_module(module_name: str, search_paths: list = None) -> Optional[types.ModuleType]:
    """
    Find and load a module using imp's search functionality.
    
    Args:
        module_name: Name of the module to find
        search_paths: List of directories to search
    
    Returns:
        Loaded module or None
    """
    try:
        # Use default sys.path if no search paths provided
        if search_paths is None:
            search_paths = sys.path
        
        # Find the module
        file_handle, pathname, description = imp.find_module(module_name, search_paths)
        
        print(f"✅ Found module: {module_name}")
        print(f"   Path: {pathname}")
        print(f"   Description: {description}")
        
        # Load the module
        module = imp.load_module(module_name, file_handle, pathname, description)
        
        # Close the file handle if it's open
        if file_handle:
            file_handle.close()
        
        return module
        
    except ImportError:
        print(f"❌ Module not found: {module_name}")
        return None
    except Exception as e:
        print(f"❌ Error loading module: {e}")
        return None


# ============================================
# DYNAMIC MODULE LOADER CLASS
# ============================================

class DynamicModuleLoader:
    """Class to handle dynamic module loading with imp"""
    
    def __init__(self, module_dirs: list = None):
        """
        Initialize module loader.
        
        Args:
            module_dirs: List of directories to search for modules
        """
        self.module_dirs = module_dirs or ['.']
        self.loaded_modules = {}
    
    def load_from_file(self, filepath: str, module_name: str = None) -> Optional[types.ModuleType]:
        """Load a module from a specific file."""
        if module_name is None:
            module_name = Path(filepath).stem
        
        if module_name in self.loaded_modules:
            print(f"ℹ️  Module already loaded: {module_name}")
            return self.loaded_modules[module_name]
        
        module = load_module_imp(filepath, module_name)
        if module:
            self.loaded_modules[module_name] = module
        return module
    
    def load_from_name(self, module_name: str) -> Optional[types.ModuleType]:
        """Load a module by name, searching in module_dirs."""
        if module_name in self.loaded_modules:
            print(f"ℹ️  Module already loaded: {module_name}")
            return self.loaded_modules[module_name]
        
        module = find_and_load_module(module_name, self.module_dirs)
        if module:
            self.loaded_modules[module_name] = module
        return module
    
    def reload_module(self, module_name: str) -> Optional[types.ModuleType]:
        """Reload a module if it's already loaded."""
        if module_name not in self.loaded_modules:
            print(f"❌ Module not loaded: {module_name}")
            return None
        
        try:
            module = self.loaded_modules[module_name]
            reloaded = imp.reload(module)
            self.loaded_modules[module_name] = reloaded
            print(f"✅ Reloaded module: {module_name}")
            return reloaded
        except Exception as e:
            print(f"❌ Error reloading module: {e}")
            return None
    
    def list_loaded_modules(self):
        """List all currently loaded modules."""
        print(f"\n📋 Loaded modules ({len(self.loaded_modules)}):")
        for name, module in self.loaded_modules.items():
            print(f"   • {name} - {module.__file__ if hasattr(module, '__file__') else 'unknown'}")
    
    def unload_module(self, module_name: str) -> bool:
        """Unload a module (remove from cache)."""
        if module_name in self.loaded_modules:
            del self.loaded_modules[module_name]
            if module_name in sys.modules:
                del sys.modules[module_name]
            print(f"✅ Unloaded module: {module_name}")
            return True
        return False


# ============================================
# CREATE SAMPLE MODULES
# ============================================

def create_sample_modules():
    """Create sample Python modules for testing."""
    
    # Create a simple math module
    math_module_content = '''"""
Sample Math Module
"""

def add(a, b):
    """Add two numbers."""
    return a + b

def subtract(a, b):
    """Subtract two numbers."""
    return a - b

def multiply(a, b):
    """Multiply two numbers."""
    return a * b

def divide(a, b):
    """Divide two numbers."""
    if b == 0:
        raise ValueError("Cannot divide by zero")
    return a / b

VERSION = "1.0.0"
AUTHOR = "Dynamic Loader Test"

def get_info():
    """Get module information."""
    return {
        'version': VERSION,
        'author': AUTHOR,
        'functions': ['add', 'subtract', 'multiply', 'divide']
    }
'''
    
    with open('math_utils.py', 'w') as f:
        f.write(math_module_content)
    print("✅ Created: math_utils.py")
    
    # Create a greeting module
    greeting_module_content = '''"""
Sample Greeting Module
"""

def greet(name):
    """Greet a person."""
    return f"Hello, {name}!"

def farewell(name):
    """Say farewell to a person."""
    return f"Goodbye, {name}!"

def welcome():
    """Welcome message."""
    return "Welcome to the dynamic module loader demo!"

__version__ = "1.0.0"
'''
    
    with open('greeting.py', 'w') as f:
        f.write(greeting_module_content)
    print("✅ Created: greeting.py")
    
    # Create a config module
    config_module_content = '''"""
Sample Configuration Module
"""

config = {
    'debug': True,
    'version': '1.0.0',
    'database': {
        'host': 'localhost',
        'port': 5432,
        'name': 'test_db'
    },
    'api_keys': {
        'service1': 'key123',
        'service2': 'key456'
    }
}

def get_config(key, default=None):
    """Get configuration value."""
    keys = key.split('.')
    value = config
    for k in keys:
        if isinstance(value, dict):
            value = value.get(k)
            if value is None:
                return default
        else:
            return default
    return value

def set_config(key, value):
    """Set configuration value."""
    keys = key.split('.')
    target = config
    for k in keys[:-1]:
        if k not in target:
            target[k] = {}
        target = target[k]
    target[keys[-1]] = value
'''
    
    with open('app_config.py', 'w') as f:
        f.write(config_module_content)
    print("✅ Created: app_config.py")


# ============================================
# DEMONSTRATION
# ============================================

def demo_imp_loading():
    """Demonstrate dynamic module loading with imp."""
    
    print("=" * 70)
    print("📦 DYNAMIC MODULE LOADING WITH IMP")
    print("=" * 70)
    
    # Create sample modules
    print("\n📁 Creating sample modules...")
    create_sample_modules()
    
    # Method 1: Load from file
    print("\n1️⃣  Loading module from file:")
    math_module = load_module_imp('math_utils.py', 'math_utils')
    
    if math_module:
        # Use the loaded module
        print(f"   Module version: {math_module.VERSION}")
        print(f"   5 + 3 = {math_module.add(5, 3)}")
        print(f"   10 - 4 = {math_module.subtract(10, 4)}")
        print(f"   6 * 7 = {math_module.multiply(6, 7)}")
        print(f"   15 / 3 = {math_module.divide(15, 3)}")
    
    # Method 2: Using the loader class
    print("\n2️⃣  Using DynamicModuleLoader:")
    loader = DynamicModuleLoader(module_dirs=['.'])
    
    # Load multiple modules
    greeting = loader.load_from_file('greeting.py')
    config = loader.load_from_name('app_config')  # Search in current dir
    
    if greeting:
        print(f"   {greeting.welcome()}")
        print(f"   {greeting.greet('Alice')}")
        print(f"   {greeting.farewell('Bob')}")
    
    if config:
        print(f"   Debug mode: {config.get_config('debug')}")
        print(f"   Database host: {config.get_config('database.host')}")
        
        # Modify config
        config.set_config('app.name', 'DynamicLoaderDemo')
        print(f"   App name: {config.get_config('app.name')}")
    
    # List loaded modules
    loader.list_loaded_modules()
    
    # Method 3: Reload a module
    print("\n3️⃣  Reloading a module:")
    # Modify the file first
    with open('math_utils.py', 'a') as f:
        f.write('\n# Added comment for reload test\n')
    
    loader.reload_module('math_utils')
    
    # Method 4: Find module by name
    print("\n4️⃣  Finding module by name:")
    sys_module = find_and_load_module('sys')
    if sys_module:
        print(f"   Found sys module: {sys_module.__file__}")
        print(f"   Python version: {sys_module.version}")
    
    # Method 5: Unload module
    print("\n5️⃣  Unloading module:")
    loader.unload_module('greeting')
    loader.list_loaded_modules()
    
    # Clean up
    print("\n🧹 Cleaning up sample files...")
    for file in ['math_utils.py', 'greeting.py', 'app_config.py']:
        if os.path.exists(file):
            os.remove(file)
            print(f"   Removed: {file}")


# ============================================
# COMPARISON WITH IMPORTLIB (MODERN APPROACH)
# ============================================

def demo_importlib_alternative():
    """Show the modern importlib approach (for comparison)."""
    
    print("\n" + "=" * 70)
    print("📦 MODERN APPROACH WITH IMPORTLIB")
    print("=" * 70)
    print("Note: imp is deprecated, use importlib instead")
    
    try:
        import importlib.util
        
        def load_with_importlib(filepath: str, module_name: str = None):
            """Load module using importlib (modern approach)."""
            filepath = os.path.abspath(filepath)
            
            if module_name is None:
                module_name = os.path.splitext(os.path.basename(filepath))[0]
            
            # Create spec and load module
            spec = importlib.util.spec_from_file_location(module_name, filepath)
            if spec is None:
                print(f"❌ Could not create spec for {filepath}")
                return None
            
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            return module
        
        # Create a test module
        with open('importlib_test.py', 'w') as f:
            f.write('''
def test_function():
    return "This was loaded with importlib"

VERSION = "2.0.0"
''')
        
        print("\n✅ Created test module for importlib")
        
        # Load with importlib
        module = load_with_importlib('importlib_test.py')
        if module:
            print(f"   Loaded module: {module.__name__}")
            print(f"   Version: {module.VERSION}")
            print(f"   Test: {module.test_function()}")
        
        # Clean up
        os.remove('importlib_test.py')
        print("🧹 Cleaned up test file")
        
    except ImportError:
        print("❌ importlib.util not available (should be in Python 3.4+)")


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command-line interface for dynamic loading."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Dynamically load Python modules with imp')
    parser.add_argument('file', nargs='?', help='Python file to load')
    parser.add_argument('-n', '--name', help='Module name (defaults to filename)')
    parser.add_argument('-f', '--function', help='Function to call after loading')
    parser.add_argument('-a', '--args', nargs='*', help='Arguments for the function')
    parser.add_argument('-c', '--call', help='Call format: module.function')
    parser.add_argument('-l', '--list', action='store_true', help='List loaded modules')
    parser.add_argument('--demo', action='store_true', help='Run demonstration')
    
    args = parser.parse_args()
    
    if args.demo:
        demo_imp_loading()
        demo_importlib_alternative()
        return
    
    if args.file:
        # Load specified file
        module = load_module_imp(args.file, args.name)
        
        if module and args.function:
            # Call specific function
            func = getattr(module, args.function, None)
            if func:
                result = func(*args.args) if args.args else func()
                print(f"Result: {result}")
            else:
                print(f"❌ Function not found: {args.function}")
        
        if module and args.call:
            # Parse module.function format
            if '.' in args.call:
                mod_name, func_name = args.call.split('.', 1)
                if mod_name == module.__name__:
                    func = getattr(module, func_name, None)
                    if func:
                        result = func(*args.args) if args.args else func()
                        print(f"Result: {result}")
                    else:
                        print(f"❌ Function not found: {func_name}")
    
    elif args.list:
        # List currently loaded modules
        print("\n📋 Loaded modules in sys.modules:")
        count = 0
        for name, module in sorted(sys.modules.items()):
            if name not in ['__main__', 'builtins'] and module is not None:
                if hasattr(module, '__file__') and module.__file__:
                    print(f"   • {name}: {module.__file__}")
                    count += 1
        print(f"\n   Total: {count} modules")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        # Run demo
        demo_imp_loading()
        demo_importlib_alternative()