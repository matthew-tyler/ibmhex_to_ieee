#   Floating Point

Matt Tyler: 1049833

Rosie Bicknell: 5886555

Will Frame: 5813315

Tony Cenaiko: 6132195


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