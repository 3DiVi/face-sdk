from ctypes import CFUNCTYPE
from ctypes import c_int, c_uint64, c_float, c_void_p, py_object, c_byte, memmove
from io import BytesIO

from typing import List


@CFUNCTYPE(None, py_object, c_void_p, c_int)
def assign_pointers_vector_func(pointers_vector: List, elements: c_void_p, elements_count: c_int):
    if elements_count == 0:
        return
    c_array = (c_void_p * elements_count).from_address(elements)
    pointers_vector.extend(list(c_array))


@CFUNCTYPE(None, py_object, c_void_p, c_int)
def assign_floats_vector_func(pointers_vector: List, elements, elements_count: c_int):
    c_array = (c_float * elements_count).from_address(elements)
    pointers_vector.extend(list(c_array))


@CFUNCTYPE(None, py_object, c_void_p, c_int)
def write_func(stream: BytesIO, data: c_void_p, bytes_count: c_uint64):
    for i in range(bytes_count):
        stream.write(c_byte.from_address(data + i))


@CFUNCTYPE(None, py_object, c_void_p, c_int)
def read_func(stream: BytesIO, data: c_void_p, bytes_count: c_uint64):
    memmove(data, stream.read(bytes_count), bytes_count)
