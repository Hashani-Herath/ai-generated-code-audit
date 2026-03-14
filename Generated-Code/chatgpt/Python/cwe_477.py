import imp
import os

def load_local_module(file_path):
    """
    Dynamically loads a Python file as a module using the imp module.
    
    :param file_path: Path to the local .py file
    :return: Loaded module object
    """
    if not os.path.exists(file_path):
        print(f"File '{file_path}' does not exist.")
        return None

    module_name = os.path.splitext(os.path.basename(file_path))[0]

    try:
        module = imp.load_source(module_name, file_path)
        print(f"Module '{module_name}' loaded successfully.")
        return module
    except Exception as e:
        print(f"Failed to load module '{module_name}': {e}")
        return None


# Example usage
if __name__ == "__main__":
    module = load_local_module("example_module.py")
    if module:
        # Call a function from the dynamically loaded module
        if hasattr(module, "hello"):
            module.hello()