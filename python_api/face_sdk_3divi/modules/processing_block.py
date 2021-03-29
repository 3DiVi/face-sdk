import sys
import json
from ctypes import c_void_p, c_char_p, create_string_buffer, POINTER, c_int32, c_int64

from .dll_handle import DllHandle
from .complex_object import ComplexObject
from .exception_check import check_exception


class ProcessingBlock(ComplexObject):
    @staticmethod
    def __serialized_ctx_to_ptr(serialized_ctx: str):
        return POINTER(c_char_p)(create_string_buffer(serialized_ctx.encode()))

    @staticmethod
    def __ptr_to_serialized_ctx(ptr: int) -> str:
        c_ptr = POINTER(c_char_p)(c_char_p(ptr))
        b_str = c_ptr.contents.value
        return b_str.decode('utf8')

    @staticmethod
    def __c_int(value: int):
        is_64bits = sys.maxsize > 2 ** 32
        if is_64bits:
            return c_int64(value)
        else:
            return c_int32(value)

    def __init__(self, dll_handle: DllHandle, config: str):
        serialized_config = json.dumps({"config": config})
        serialized_config_ptr = self.__serialized_ctx_to_ptr(serialized_config)

        impl = dll_handle.TDVFaceAttributesEstimator_createByConfig(serialized_config_ptr)
        super(ProcessingBlock, self).__init__(dll_handle, c_void_p(impl))
        check_exception(self.get_exception(), self._dll_handle)

    def process(self, ctx: dict):
        c_serialized_impl_ptr = self.__c_int(self._impl.value)

        serialized_ctx = json.dumps(ctx)
        c_serialized_ctx_ptr = self.__serialized_ctx_to_ptr(serialized_ctx)

        new_ctx_ptr = self._dll_handle.TDVProcessingBlock_processSparse(c_serialized_impl_ptr, c_serialized_ctx_ptr)
        check_exception(self.get_exception(), self._dll_handle)

        serialized_ctx = self.__ptr_to_serialized_ctx(new_ctx_ptr)

        self.__free_str(new_ctx_ptr)

        new_ctx = json.loads(serialized_ctx)
        return new_ctx

    def get_exception(self) -> c_void_p:
        c_serialized_impl_ptr = self.__c_int(self._impl.value)
        exception_ptr = self._dll_handle.TDVProcessingBlock_getException(c_serialized_impl_ptr)
        return POINTER(c_void_p)(c_void_p(exception_ptr))

    def __free_str(self, ptr: int):
        c_ptr = self.__c_int(ptr)
        self._dll_handle.tdvFreeStr(c_ptr)

    def __del__(self):
        self._dll_handle.TDVProcessingBlock_destroy(
            c_void_p(self._obj_impl)
        )
        self._obj_impl = None
