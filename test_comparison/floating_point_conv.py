"""
IBM to IEEE Floating-Point Converter

This Python program reads an input binary file containing IBM floating-point values in single or double format,
converts them to IEEE floating-point values in single or double format, and writes the converted values to an output
binary file.

Author: Tony Cenaiko
Date: 24/05/23

Usage:
    Enter name of input file: [input_file]
    Enter input type ('s' for single point or 'd' for double point): [input_data_type]
    Enter name of output file: [output_file]
    Enter output type ('s' for single point or 'd' for double point): [output_data_type]

    - input_file: The name of the input binary file containing IBM floating-point values.
    - output_file: The name of the output binary file to write the converted IEEE floating-point values. Do not include file extension.
    - data_type: The data type to convert. Use 's' for single precision or 'd' for double precision.

Example:
    Enter name of input file: testIBMSingle.bin
    Enter input type ('s' for single point or 'd' for double point): d
    Enter name of output file: test
    Enter output type ('s' for single point or 'd' for double point): d
"""
import struct
import binascii
import numpy as np
import os


chunk_size_32 = 4
chunk_size_64 = 8
MAX_SINGLE = 3.4028235 * (10**38)
MAX_DOUBLE = 1.7976931348623157 * (10**308)
floats = []


def get_bin_str_32(binary_data):
    """
    Converts binary data to a binary string representation of 32 bits.

    Parameters:
        binary_data (bytes): The binary data to be converted.

    Returns:
        str: The binary string representation of the input data, padded to 32 bits with leading zeros.

    Example:
        binary_data = b'\x00\x00\x80\x3F'
        bin_str = get_bin_str_32(binary_data)
        print(bin_str)  # Output: '00111111100000000000000000000000'
    """
    hex_string = binascii.hexlify(binary_data).decode()
    bin_num = int(hex_string, 16)
    bin_str = format(bin_num, '0>32b')
    return bin_str


def get_bin_str_64(binary_data):
    """
    Converts binary data to a binary string representation of 64 bits.

    Parameters:
        binary_data (bytes): The binary data to be converted.

    Returns:
        str: The binary string representation of the input data, padded to 64 bits with leading zeros.

    Example:
        binary_data = b'\x00\x00\x00\x00\x00\x00\xf0\x3f'
        bin_str = get_bin_str_64(binary_data)
        print(bin_str)  # Output: '0011111111000000000000000000000000000000000000000000000000000000'
    """
    hex_string = binascii.hexlify(binary_data).decode()
    bin_num = int(hex_string, 16)
    bin_str = format(bin_num, '0>64b')
    return bin_str


def str_to_ibm_flt_32(bin_str):
    """
    Converts a binary string representation of an IBM float to its decimal value (single precision).

    Parameters:
        bin_str (str): The binary string representing the IBM float.

    Returns:
        float: The decimal value of the IBM float.

    Example:
        bin_str = '01000000111100000000000000000000'
        result = str_to_ibm_flt_32(bin_str)
        print(result)  # Output: 4.0
    """
    sgn_bit = bin_str[0]
    exp_bits = bin_str[1:8]
    mts_bits = bin_str[8:]
    
    exp_dec = int(exp_bits, 2)
    mts_dec = int(mts_bits, 2)
    sgn_dec = 1
    if sgn_bit == "1":
        sgn_dec = -1
    
    output = sgn_dec * (16**(exp_dec - 64)) * (mts_dec / (2**24))
    return output


def str_to_ibm_flt_64(bin_str):
    """
    Converts a binary string representation of an IBM float to its decimal value (double precision).

    Parameters:
        bin_str (str): The binary string representing the IBM float.

    Returns:
        float: The decimal value of the IBM float.

    Example:
        bin_str = '0100000000111100000000000000000000000000000000000000000000000000'
        result = str_to_ibm_flt_64(bin_str)
        print(result)  # Output: 3.5
    """
    sgn_bit = bin_str[0]
    exp_bits = bin_str[1:8]
    mts_bits = bin_str[8:]
    
    exp_dec = int(exp_bits, 2)
    mts_dec = int(mts_bits, 2)
    sgn_dec = 1
    if sgn_bit == "1":
        sgn_dec = -1
    
    output = sgn_dec * (16**(exp_dec - 64)) * (mts_dec / (2**56))
    return output


def convert_double_to_single(double_value):
    """
    Converts a double precision floating-point value to single precision.

    Parameters:
        double_value (float): The double precision floating-point value to convert.

    Returns:
        float: The single precision floating-point value.

    Example:
        double_val = 3.14159
        single_val = convert_double_to_single(double_val)
        print(single_val)  # Output: 3.14159
    """
    double_array = np.array([double_value], dtype=np.float64)  # Create a numpy array with the double value
    single_array = double_array.astype(np.float32)  # Convert the numpy array to single precision

    single_value = single_array[0]  # Extract the single precision value from the array

    return single_value


def write_floats_to_binary_file(floats, filename):
    """
    Writes a list of floating-point values to a binary file.

    Parameters:
        floats (list): A list of floating-point values to write.
        filename (str): The name of the binary file to create/write.

    Returns:
        None

    Example:
        data = [3.14, 1.618, 2.718]
        write_floats_to_binary_file(data, 'output.bin')
    """
    with open(filename, 'wb') as file:
        for f in floats:
            binary_data = struct.pack('!f', f)  # Pack float into binary format
            file.write(binary_data)  # Write binary data to file
            
            hex_data = binascii.hexlify(binary_data)


def write_doubles_to_binary_file(doubles, filename):
    """
    Writes a list of double precision floating-point values to a binary file.

    Parameters:
        doubles (list): A list of double precision floating-point values to write.
        filename (str): The name of the binary file to create/write.

    Returns:
        None

    Example:
        data = [3.14, 1.618, 2.718]
        write_doubles_to_binary_file(data, 'output.bin')
    """
    with open(filename, 'wb') as file:
        for d in doubles:
            d = float(d)
            binary_data = struct.pack('!d', d)  # Pack double into binary format
            file.write(binary_data)  # Write binary data to file


input_file = input("Enter name of input file: ")
input_type = input("Enter input type ('s' for single point or 'd' for double point): ")
output_file = input("Enter name of output file: ") + ".bin"
output_type = input("Enter output type ('s' for single point or 'd' for double point): ")


while True:
    file_exists =  os.path.exists(input_file)
    input_valid = (input_type == "s") or (input_type == "d")
    output_valid = (output_type == "s") or (output_type == "d")
    if not file_exists:
        input_file = input("Invalid filename. Enter name of input file: ")
    elif not input_valid:
        input_type = input("Invalid input type. Enter 's' for single point or 'd' for double point: ")
    elif not output_valid:
        output_type = input("Invalid output type. Enter 's' for single point or 'd' for double point: ")
    else:
        break


if input_type == "s":
    with open(input_file, 'rb') as file:
        while True:
            chunk = file.read(chunk_size_32)
            if not chunk:
                break
            
            bin_str = get_bin_str_32(chunk)
            
            if output_type == "s":
                ibm_flt = str_to_ibm_flt_32(bin_str)
                
                if ibm_flt > MAX_SINGLE:
                    ibm_flt = 0
                
                flt32 = convert_double_to_single(ibm_flt)
                floats.append(flt32)
            elif output_type == "d":
                bin_str += (32*"0")
                ibm_flt = str_to_ibm_flt_64(bin_str)
                floats.append(ibm_flt)
elif input_type == "d":
    with open(input_file, 'rb') as file:
        while True:
            chunk = file.read(chunk_size_64)
            if not chunk:
                break
            
            bin_str = get_bin_str_64(chunk)
            
            if output_type == "d":
                ibm_flt = str_to_ibm_flt_64(bin_str)
                
                if ibm_flt > MAX_DOUBLE:
                    ibm_flt = 0
                
                floats.append(ibm_flt)
            elif output_type == "s":
                bin_str = bin_str[:32]
                ibm_flt = str_to_ibm_flt_32(bin_str)
                
                if ibm_flt > MAX_SINGLE:
                    ibm_flt = 0
                    
                flt32 = convert_double_to_single(ibm_flt)
                floats.append(flt32)


if output_type == "s":
    write_floats_to_binary_file(floats, output_file)
elif output_type == "d":
    write_doubles_to_binary_file(floats, output_file)

    

















































