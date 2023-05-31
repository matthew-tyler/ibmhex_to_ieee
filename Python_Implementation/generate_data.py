import numpy as np
import segpy.ibm_float as ibm

# Open files (binary = the binary input for our program, output = expected output)
binary = open('ibm_floats.bin', 'wb')
output = open('output.txt', 'w')

# For tests in our ranges,
for i in range(-100, 101):
    # Write the binary bytes to the binary file...
    binary.write(ibm.ieee2ibm(i))   
    # ...and the expected output to the output file.
    output.write(str(ibm.IBMFloat.from_float(i)) + "\n") 
for i in np.arange(-1000, 1000, 7.6543):
    binary.write(ibm.ieee2ibm(i))
    output.write(str(ibm.IBMFloat.from_float(i)) + "\n")

# Close each file to ensure everything is saved correctly.
binary.close()
output.close()