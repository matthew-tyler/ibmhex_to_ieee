#   Floating Point

Matt Tyler: 1049833
Rosie Bicknell: 5886555
Will Frame: 5813315
Tony Cenaiko: 6132195

Date: 24/05/23


## Overview
Our primary implementation is floating.c located in /src. However, we found that floating.c was producing small rounding errors, and so we wrote a python implementation of Floating Point that we could use to test floating.c against. Our python implementation is the floating_point_conv.py file in /Python_Implementation.


## Building and Running


To build and run the program using the provided Makefile, you can use the following steps in a terminal:

1. Navigate to the project directory using the cd command:

    ```bash
    cd path/to/project
    ```

2. Type make in the terminal to build the program:

    ```bash
    make
    ```

    This will compile the program and generate the executable file.

3. Run the program by typing:

    ```bash
    ./bin/floating  [path/to/input] [-s or -d]
    ```

For example with our program and test data,

```bash
./bin/floating  ./testdata/ibm_floats.bin -s
```

## Generating Test Data - Will Frame

For generating test data I discovered [SegPy](https://segpy.readthedocs.io/en/latest/apidocs/segpy/segpy.ibm_float.html). SegPy supports converting IEEE floats to IBM single precision floats. This worked for a large range of tests I generated using [generate_data.py](testdata/generate_data.py), however, there were slight differences in the output which seemed to be from a rounding issue. As the formula for double precision was basically the same, and I couldn't find a library for converting IEEE to IBM double precision, we just tested them by hand and it seemed to work to a similar degree for each of our tests.


## Python Implementation - Tony Cenaiko

floating_point_conv.py reads through a binary file by either 4 or 8-byte chunks, depending on whether it's interpreting the file contents as floats or doubles. It then turns each chunk of binary data into its corresponding string of 1s and 0s. After that, it's mostly just straightforward string manipulation with a bit of math thrown in. We grab the sign bit, the exponent and the mantissa, and perform the function:
new_float_wossname = 16^(exponent - 64) * (mantissa / 2^24)
and multiply that by -1 if the sign bit == "1". If it's a double, then 2^24 becomes 2^56. Easy peasy.

At this point new_float_wossname is a 64-bit double (because python represents all floats as 64-bit doubles). So we just get the ieee binary for new_float_wossname and write it into a binary file. If the IBM floats are outside the allowed range for the type of IEEE they're being written to, the value of that float is set to 0.

We used generate_data.py to write IBM floats to get the IBM representation of all integers between -100 and 100, and a range of rational numbers between -1000 and 1000. We then ran that bin file through floating_point_conv. The output bin file was then run through bin_to_ieee.py, which just prints out a binary file's contents as ieee floats. The decimal representation of IBM floats were then compared to the decimal representation of the IEEE floats. And lo, the Lord saw that the input matched up with the output, and said that it was good.

### How to Run Python Implementation

The documentation in floating_point_conv.py's header explains how to run the program. The required modules are as follows:
import struct
import binascii
import numpy as np
import os

### Credits

I used ChatGPT to help with a lot of the nitty-gritty python nonsense, such as how to turn a hex string into a binary string. And I also used ChatGPT to write the documentation for the program.

## Challenges

We weren't able to find any resources online where you can turn a hex or binary string, which represents an IBM float, into its corresponding decimal value, so we had to do those conversions by hand.

We're still not sure why floating.c has small rounding errors. There's also the difficulty of having no authoritative answer on what an IBM float should translate to in decimal.