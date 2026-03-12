import imp

# Define the path to the Python file to load
file_path = 'example.py'

try:
    # Load the Python file using the imp module
    mod = imp.load_source('example', file_path)

    # Call the function defined in the loaded file
    print(mod.example_function())

except ImportError:
    print(f"Error loading file: {file_path}")

except Exception as e:
    print(f"Error: {e}")
