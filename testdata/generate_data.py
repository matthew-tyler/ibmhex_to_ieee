import struct
import numpy as np

binary = open('ibm_floats.bin', 'wb')
floats = open('output.txt', 'w')
for i in range(1, 101):
    ibm_float = struct.pack('>f', i)
    binary.write(ibm_float)
    floats.write(str(i) + "\n")

for i in np.arange(1, 1000, 7.156):
    ibm_float = struct.pack('>f', i)
    binary.write(ibm_float)
    floats.write(str(i) + "\n")

binary.close()
floats.close()
