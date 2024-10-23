from ctypes import c_int32, c_int64, c_void_p

from .resizable_templates_index import ResizableTemplatesIndex
from .recognizer import Recognizer
from .dll_handle import DllHandle
from .exception_check import check_exception, make_exception


class ResizableRecognizer(Recognizer):
    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super().__init__(dll_handle, impl)

    def create_resizable_templates_index(self, templates_count: int,
                                         search_threads_count: int) -> ResizableTemplatesIndex:
        exception = make_exception()

        result_impl = self._dll_handle.Recognizer_createResizableIndex(
            self._impl,
            c_int64(templates_count),
            c_int32(search_threads_count),
            exception)

        check_exception(exception, self._dll_handle)

        return ResizableTemplatesIndex(self._dll_handle, c_void_p(result_impl))
