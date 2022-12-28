##
#  \file face_attributes_estimator.py
#  \~English
#     \brief ProcessingBlock is an interface object used to work with estimators from Processing Block API.
#  \~Russian
#     \brief ProcessingBlock - интерфейсный объект для взаимодействия с методами из Processing Block API.

import sys
import json
from ctypes import c_void_p, c_char_p, create_string_buffer, POINTER, c_int32, c_int64
from typing import Union

from .complex_object import ComplexObject
from .exception_check import check_exception, check_processing_block_exception, make_exception
from .dll_handle import DllHandle
from .context import Context
from .error import Error

## @defgroup PythonAPI
#  @{
## @defgroup ProcessingBlock
#  @{
    
##
#  \~English
#     \brief Interface object used to work with estimators from Processing Block API.
#  \~Russian
#     \brief Интерфейсный объект для взаимодействия с методами из Processing Block API.

class ProcessingBlock(ComplexObject):
    def __del__(self):
        exception = make_exception()

        self._dll_handle.TDVProcessingBlock_destroyBlock(self._impl, exception)

        check_exception(exception, self._dll_handle)

    def __call__(self, ctx: Union[dict, Context]):
        if isinstance(ctx, dict):
            self.__call_dicts(ctx)
        elif isinstance(ctx, Context):
            self.__call_ctx(ctx)
        else:
            raise Error(0xa341de35, "Wrong type of ctx")

    def __call_dicts(self, ctx: dict):
        exception = make_exception()
        meta_ctx = Context(self._dll_handle)
        meta_ctx(ctx)

        self._dll_handle.TDVProcessingBlock_processContext(self._impl, meta_ctx._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)

        new_keys_dict = set(meta_ctx.keys()) - set(ctx.keys())
        for key in new_keys_dict:
            ctx[key] = self.get_output_data(meta_ctx[key])

    def __call_ctx(self, ctx: Context):
        exception = make_exception()

        self._dll_handle.TDVProcessingBlock_processContext(self._impl, ctx._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)

    def get_output_data(self, meta_ctx: Context):

        if meta_ctx.is_array():
            return [self.get_output_data(meta_ctx[i]) for i in range(len(meta_ctx))]

        if meta_ctx.is_object():
            return {key: self.get_output_data(meta_ctx[key]) for key in meta_ctx.keys()}

        return meta_ctx.get_value()

##
#  \~English
#     \brief Interface object used to work with Legacy Processing Block API. Shouldn't be used anymore.
#  \~Russian
#     \brief Интерфейсный объект для взаимодействия с методами из Processing Block API.

class LegacyProcessingBlock(ComplexObject):
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
        super(LegacyProcessingBlock, self).__init__(dll_handle, c_void_p(impl))
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
        self._dll_handle.TDVProcessingBlock_destroy(self._impl)
