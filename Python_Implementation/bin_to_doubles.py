import struct

def read_doubles_from_binary_file(filename):
    with open(filename, 'rb') as file:
        # Read the binary data from the file
        binary_data = file.read()

        # Calculate the number of doubles in the binary data
        num_doubles = len(binary_data) // struct.calcsize('d')

        # Unpack the binary data into doubles
        doubles = struct.unpack('d' * num_doubles, binary_data)

        return doubles

doubles = read_doubles_from_binary_file('s_to_d.bin')
for d in doubles:
    print(d)
#print(doubles)