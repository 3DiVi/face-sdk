from ctypes import CDLL, c_int64, POINTER, c_char_p, c_void_p, py_object, pointer, c_uint64
from io import BytesIO
from typing import List

from .templates_index import TemplatesIndex, Template
from .exception_check import make_exception, check_exception
from .wrap_funcs import write_func


class ResizableTemplatesIndex(TemplatesIndex):
    def __init__(self, dll_handle: CDLL, impl: c_void_p):
        super().__init__(dll_handle, impl)

    def add(self, templates: List[Template], uuids: List[str]):
        exception = make_exception()

        templates_impls = (c_void_p * len(templates))()
        uuids_array = (c_char_p * len(templates))()

        for index in range(len(templates)):
            templates_impls[index] = templates[index]._impl
            uuids_array[index] = uuids[index].encode()

        self._dll_handle.ResizableTemplatesIndex_add_2(
            self._impl,
            templates_impls,
            pointer(uuids_array),
            c_uint64(len(templates)),
            exception)

        check_exception(exception, self._dll_handle)

    def remove(self, uuids: List[str]):
        exception = make_exception()

        uuids_array = (c_char_p * len(uuids))()

        for index in range(len(uuids)):
            uuids_array[index] = uuids[index].encode()

        self._dll_handle.ResizableTemplatesIndex_remove_2(
            self._impl,
            pointer(uuids_array),
            c_uint64(len(uuids)),
            exception)

        check_exception(exception, self._dll_handle)

    def capacity(self) -> int:
        exception = make_exception()

        capacity = self._dll_handle.ResizableTemplatesIndex_capacity(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return capacity

    def concat(self, index: "ResizableTemplatesIndex"):
        exception = make_exception()

        self._dll_handle.ResizableTemplatesIndex_concatenate(
            self._impl,
            index._impl,
            exception)

        check_exception(exception, self._dll_handle)

    def at_by_uuid(self, uuid: str) -> Template:
        exception = make_exception()

        result_impl = self._dll_handle.ResizableTemplatesIndex_at_by_uuid(
            self._impl,
            c_char_p(uuid.encode()),
            exception)

        check_exception(exception, self._dll_handle)

        return Template(self._dll_handle, c_void_p(result_impl))

    def at_by_index(self, i: int) -> str:
        stream = BytesIO()

        exception = make_exception()

        self._dll_handle.ResizableTemplatesIndex_at_by_index(
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
