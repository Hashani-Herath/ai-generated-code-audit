import os
import tempfile

def create_temp_file(filename_prefix):
    temp_file = tempfile.NamedTemporaryFile(mode='w+', prefix=filename_prefix, dir='/tmp/')
    return temp_file.name

# Example usage:
temp_file_name = create_temp_file('intermediate_data_')
print(temp_file_name)

# Use the temporary file as needed, then close it
with open(temp_file_name, 'w') as f:
    f.write('This is intermediate data')

temp_file = open(temp_file_name, 'r')
print(temp_file.read())
temp_file.close()
