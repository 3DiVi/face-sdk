from ctypes import POINTER
from ctypes import c_char_p, c_uint, c_void_p
import locale

from .dll_handle import DllHandle
from .error import Error


class ExceptionDestroyer:
    def __init__(self, exception: c_void_p, dll_handle: DllHandle):
        self.__exception = exception
        self._dll_handle = dll_handle

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.__exception.contents:
            self._dll_handle.apiObject_destructor(self.__exception.contents)

class ProcessngBlockExceptionDestroyer:
    def __init__(self, exception: c_void_p, dll_handle: DllHandle):
        self.__exception = exception
        self._dll_handle = dll_handle

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.__exception.contents:
            self._dll_handle.apiProcessingBlockObject_destructor(self.__exception.contents)


def check_exception(exception: c_void_p, dll_handle: DllHandle):
    with ExceptionDestroyer(exception, dll_handle):
        if exception.contents:
            what = dll_handle.apiException_what(exception.contents)
            what = c_char_p(what).value.decode(locale.getpreferredencoding())

            code = dll_handle.apiException_code(exception.contents)
            code = hex(c_uint(code).value)
            raise Error(code, what)

def check_processing_block_exception(exception: c_void_p, dll_handle: DllHandle):
    with ProcessngBlockExceptionDestroyer(exception, dll_handle):
        if exception.contents:
            what = dll_handle.apiProcessingBlockException_what(exception.contents)
            what = c_char_p(what).value.decode(locale.getpreferredencoding())

            code = dll_handle.apiProcessingBlockException_code(exception.contents)
            code = hex(c_uint(code).value) if code else code
            raise Error(code, what)



def make_exception():
    return POINTER(c_void_p)(POINTER(c_void_p)())
