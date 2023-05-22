import struct
import numpy as np
import segpy.ibm_float as ibm

binary = open('ibm_floats.bin', 'wb')
output = open('output.txt', 'w')
for i in range(1, 101):
    binary.write(ibm.ieee2ibm(i))
    output.write(f'{i}\n')
for i in np.arange(1, 1000, 7.2):
    binary.write(ibm.ieee2ibm(i))
    output.write(f'{i}\n')
