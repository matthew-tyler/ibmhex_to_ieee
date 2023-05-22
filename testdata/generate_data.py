import numpy as np
import segpy.ibm_float as ibm

binary = open('ibm_floats.bin', 'wb')
output = open('output.txt', 'w')

for i in range(1, 101):
    binary.write(ibm.ieee2ibm(i))
    output.write(str(i) + "\n")
for i in np.arange(1, 1000, 7.6543):
    binary.write(ibm.ieee2ibm(i))
    output.write(str(i) + "\n")

binary.close()
output.close()
