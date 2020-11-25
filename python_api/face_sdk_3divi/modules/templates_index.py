##
#  \file TemplatesIndex.py
#  \~English
#     \brief TemplatesIndex - Interface object for working with the template index.
#  \~Russian
#     \brief TemplatesIndex - Интерфейсный объект для работы с индексом шаблонов.

from ctypes import CDLL
from ctypes import c_void_p, py_object

from io import BytesIO

from .exception_check import check_exception, make_exception
from .wrap_funcs import write_func
from .complex_object import ComplexObject
from .template import Template


##
# \~English
#   \brief Interface object for working with the template index.
# \~Russian
#   \brief Интерфейсный объект для работы с индексом шаблонов.
class TemplatesIndex(ComplexObject):

    def __init__(self, dll_handle: CDLL, impl: c_void_p):
        super(TemplatesIndex, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Get a method name.
    #      Thread-safe.
    #
    #    \return The name of the method created this index.
    #
    # \~Russian
    #    \brief Получить имя метода.
    #      Потокобезопасный.
    #
    #    \return Имя метода, создавшего этот шаблон.
    def get_method_name(self) -> str:
        name_stream = BytesIO()

        exception = make_exception()

        self._dll_handle.TemplatesIndex_getMethodName(
            self._impl,
            py_object(name_stream),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)

        name = name_stream.getvalue()
        name_stream.close()

        return name.decode()

    ##
    # \~English
    #    \brief Get a number of templates in the index.
    #      Thread-safe.
    #
    #    \return Number of templates in the index.
    #
    # \~Russian
    #    \brief Получить количество шаблонов в индексе.
    #      Потокобезопасный.
    #
    #    \return Количество шаблонов в индексе.
    def size(self) -> int:
        exception = make_exception()

        size = self._dll_handle.TemplatesIndex_size(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return size

    ##
    # \~English
    #    \brief Get an i-th template.
    #      Thread-safe.
    #
    #    \param[in] i
    #      Integer i: 0 <= i < size.
    #
    #    \return The i-th template.
    #
    # \~Russian
    #    \brief Получить i-й шаблон.
    #      Потокобезопасный.
    #
    #    \param[in] i
    #      Целое i: 0 <= i < size.
    #
    #    \return i-й шаблон.
    def at(self, i: int) -> Template:
        exception = make_exception()

        result_impl = self._dll_handle.TemplatesIndex_at(
            self._impl,
            i,
            exception)

        check_exception(exception, self._dll_handle)

        return Template(self._dll_handle, c_void_p(result_impl))
