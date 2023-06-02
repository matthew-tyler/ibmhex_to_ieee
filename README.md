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
    ./bin/floating 
    ```


## C Implementation & Overflows - Matt Tyler

The basic approach to this is to take the formula for representing both IBM and IEEE and convert from one to the other. Using a bit of C magic, we can access the bit fields as required nice and cleanly using a union. The conversion itself is very straightforward, simply converting each field back to decimal, then setting the fraction in an empty double and using ldexp to load the new exponent against that fraction. This way, the hard work is done by someone else, which is the best way to code. This is in contrast to the print method, which was the original approach I took. This works as well, but I believe it is less accurate due to entirely relying on floating point maths. 

Currently overflows are simply set to 0, but this can be easily changed.

The program assumes the endianness of the file to be big endian, this is because of the IBM /360 systems it's meant to have been coming from. It also will determine the endianness of the system it is running on and handle correctly. 

There is some challenge with regard to portability in the C implementation, as it was having some issues on windows, possibly due to struct packing. I have attempted to address this with the pragma, but not been able to confirm if it worked.

Finally being able to confirm the accuracy has been a problem. The output is different to the test files we're able to generate, however the test files were generated with a python library.

If the python library took a similar approach however did its math using arbitrary precision integers, it would have a different accuracy up until it handles the fraction. This could be the source of differences, however I am unable to confirm which is more accurate. 

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