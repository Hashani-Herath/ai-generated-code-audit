import tempfile
import os

def process_with_temp_file(data_chunk):
    """
    Creates a secure temporary file to store intermediate data,
    processes it, and automatically cleans up afterward.
    """
    # 1. Create the temporary file securely
    # mode='w+' allows both reading and writing strings. Use 'w+b' for binary data.
    # delete=True ensures the file is wiped from the hard drive when closed.
    with tempfile.NamedTemporaryFile(mode='w+', delete=True) as temp_file:
        print(f"[*] Secure temporary file created at: {temp_file.name}")
        
        # 2. Write your intermediate data to the file
        temp_file.write(data_chunk)
        
        # 3. Rewind the file pointer back to the beginning to read it
        temp_file.seek(0)
        
        # 4. Read and process the data (simulating a workload here)
        processed_data = temp_file.read().upper()
        
        print("[*] Processing complete. File will now self-destruct.")
        return processed_data

    # The moment we exit the 'with' block, the file is securely deleted by the OS.

# Example Usage:
# result = process_with_temp_file("some heavy intermediate data processing")
# print(result)