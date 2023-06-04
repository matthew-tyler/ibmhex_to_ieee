import numpy as np
import segpy.ibm_float as ibm

# Open files (binary = the binary input for our program, output = expected output)
binary = open('ibm_floats.bin', 'wb')
output = open('output.txt', 'w')

# For tests in our ranges,
for i in range(1, 101):
    # Write the binary bytes to the binary file and the expected output to the output file.
    output.write(str(ibm.IBMFloat.from_float(i)) + "\n")
    binary.write(ibm.ieee2ibm(ibm.IBMFloat.from_float(i))) 
for i in np.arange(1, 1000, 7.6543):
    output.write(str(ibm.IBMFloat.from_float(i)) + "\n")
    binary.write(ibm.ieee2ibm(ibm.IBMFloat.from_float(i))) 

# Close each file to ensure everything is saved correctly.
binary.close()
output.close()