import struct

def read_ieee_floats_from_binary_file(filename):
    with open(filename, 'rb') as file:
        binary_data = file.read()
    
    float_size = struct.calcsize('f')
    num_floats = len(binary_data) // float_size
    
    floats = []
    for i in range(num_floats):
        float_data = binary_data[i * float_size : (i + 1) * float_size]
        float_value = struct.unpack('!f', float_data)[0]
        floats.append(float_value)
    
    return floats

# Example usage
input_file = 'ieee_floats2.bin'
ieee_floats = read_ieee_floats_from_binary_file(input_file)
#print(ieee_floats)
for f in ieee_floats:
    print(f)