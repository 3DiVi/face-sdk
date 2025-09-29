from ctypes import c_int64, c_char_p, c_void_p, py_object, pointer, c_uint64
from io import BytesIO
from typing import List, Union

from multipledispatch import dispatch

from .dll_handle import DllHandle
from .templates_index import Template
from .context_template import ContextTemplate
from .exception_check import make_exception, check_exception
from .wrap_funcs import write_func


class DynamicTemplateIndex:
    def __init__(self, dll_handle: DllHandle, impl: c_void_p, weak: bool = False):
        self._dll_handle = dll_handle
        self._impl = impl
        self._weak = weak

    @dispatch(ContextTemplate, str)
    def add(self, template: ContextTemplate, uuid: str):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_add_3(
            self._impl,
            template._impl,
            uuid.encode(),
            exception
        )

        check_exception(exception, self._dll_handle)

    @dispatch(Template, str)
    def add(self, template: Template, uuid: str):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_add_1(
            self._impl,
            template._impl,
            uuid.encode(),
            exception
        )

        check_exception(exception, self._dll_handle)

    @dispatch(list, list)
    def add(self, templates: Union[List[Template], List[ContextTemplate]], uuids: List[str]):
        exception = make_exception()

        templates_impls = (c_void_p * len(templates))()
        uuids_array = (c_char_p * len(uuids))()

        for index in range(len(templates)):
            templates_impls[index] = templates[index]._impl
            uuids_array[index] = uuids[index].encode()

        if isinstance(templates[0], Template):
            self._dll_handle.DynamicTemplateIndex_add_2(
                self._impl,
                templates_impls,
                pointer(uuids_array),
                c_uint64(len(templates)),
                exception)
        elif isinstance(templates[0], ContextTemplate):
            self._dll_handle.DynamicTemplateIndex_add_4(
                self._impl,
                templates_impls,
                pointer(uuids_array),
                c_uint64(len(templates)),
                exception)
        else:
            raise NotImplementedError("Wrong template type")

        check_exception(exception, self._dll_handle)

    @dispatch(str)
    def remove(self, uuid: str):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_remove_1(
            self._impl,
            uuid.encode(),
            exception
        )

        check_exception(exception, self._dll_handle)

    @dispatch(list)
    def remove(self, uuids: List[str]):
        exception = make_exception()

        uuids_array = (c_char_p * len(uuids))()

        for index in range(len(uuids)):
            uuids_array[index] = uuids[index].encode()

        self._dll_handle.DynamicTemplateIndex_remove_2(
            self._impl,
            pointer(uuids_array),
            c_uint64(len(uuids)),
            exception)

        check_exception(exception, self._dll_handle)

    def size(self) -> int:
        exception = make_exception()

        size = self._dll_handle.DynamicTemplateIndex_size(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return size

    def capacity(self) -> int:
        exception = make_exception()

        capacity = self._dll_handle.DynamicTemplateIndex_capacity(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return capacity

    def concat(self, index: "DynamicTemplateIndex"):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_concatenate(
            self._impl,
            index._impl,
            exception)

        check_exception(exception, self._dll_handle)

    def clear(self):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_clear(self._impl, exception)

        check_exception(exception, self._dll_handle)
        
    def save(self, file_path: str, allow_overwrite: bool):
        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_save(self._impl, c_char_p(file_path.encode()), allow_overwrite, exception)
        
        check_exception(exception, self._dll_handle)

    @dispatch(str)
    def at(self, uuid: str) -> ContextTemplate:
        exception = make_exception()

        result_impl = self._dll_handle.DynamicTemplateIndex_at_by_uuid(
            self._impl,
            c_char_p(uuid.encode()),
            exception)

        check_exception(exception, self._dll_handle)

        return ContextTemplate(self._dll_handle, c_void_p(result_impl))

    def get_uuid(self, i: int) -> str:
        stream = BytesIO()

        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_at_by_index(
            self._impl,
            c_int64(i),
            py_object(stream),
            write_func,
            exception
        )

        check_exception(exception, self._dll_handle)

        uuid = stream.getvalue()
        stream.close()

        return uuid.decode()

    def get(self, index: int) -> ContextTemplate:
        exception = make_exception()

        result_impl = self._dll_handle.DynamicTemplateIndex_get(
            self._impl,
            c_int64(index),
            exception)

        check_exception(exception, self._dll_handle)

        return ContextTemplate(self._dll_handle, c_void_p(result_impl))

    def get_method_name(self) -> str:
        name_stream_wrap = BytesIO()

        exception = make_exception()

        self._dll_handle.DynamicTemplateIndex_getMethodName(
            self._impl,
            py_object(name_stream_wrap),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)

        name = name_stream_wrap.getvalue()
        name_stream_wrap.close()
        return name.decode()

    def __del__(self):
        if self._impl is not None and not self._weak:
            self._dll_handle.DynamicTemplateIndex_destructor(
                self._impl
            )

        self._impl = None
