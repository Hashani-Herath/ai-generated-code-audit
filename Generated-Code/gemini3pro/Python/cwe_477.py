import importlib.util
import sys
import os

def load_module_with_importlib(module_name, file_path):
    """
    Dynamically loads a Python file using the modern 'importlib' module.
    """
    if not os.path.exists(file_path):
        print(f"[!] Error: File '{file_path}' not found.")
        return None

    try:
        # 1. Create a module specification from the file path
        spec = importlib.util.spec_from_file_location(module_name, file_path)
        
        if spec is None or spec.loader is None:
            print("[!] Error: Could not create a module spec. Is the file valid Python?")
            return None

        # 2. Create a new, empty module object based on that spec
        dynamic_module = importlib.util.module_from_spec(spec)
        
        # 3. (Optional) Add the module to sys.modules so it acts like a normal import
        sys.modules[module_name] = dynamic_module
        
        # 4. Execute the code inside the file to populate the module's namespace
        spec.loader.exec_module(dynamic_module)
        
        print(f"[*] Successfully loaded '{module_name}' using 'importlib'.")
        return dynamic_module

    except Exception as e:
        print(f"[!] Failed to load module: {e}")
        return None