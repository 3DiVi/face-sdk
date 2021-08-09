##
#  \file TemplatesIndex.py
#  \~English
#     \brief TemplatesIndex - Interface object for working with the template index.
#  \~Russian
#     \brief TemplatesIndex - Интерфейсный объект для работы с индексом шаблонов.

from ctypes import CDLL, c_int64
from ctypes import c_void_p, py_object

from io import BytesIO

from .exception_check import check_exception, make_exception
from .wrap_funcs import write_func
from .complex_object import ComplexObject
from .template import Template

## @defgroup PythonAPI
#  @{
## @defgroup TemplatesIndex
#  @{

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

    ##
    # \~English
    #    \brief Reserve memory for temporary buffers used when searching.
    #      These temporary buffers require 8 * size() * queries_count bytes.
    #
    #    \param[in]  queries_count
    #      Integer queries_count >= 0 - max size of the queries_templates list
    #      passed to Recognizer.search at once to prepare the buffers for.
    #
    # \~Russian
    #    \brief Зарезервировать память для временных буферов, используемых при поиске.
    #       Эти временные буферы требуют 8 * size() * queries_count байт.
    #
    #    \param[in]  queries_count
    #      Целое число queries_count >= 0 - максимальный размер списка
    #      queries_templates, подаваемого в Recognizer.search за один раз,
    #      к которому нужно подготовить буферы.
    def reserve_search_memory(self, queries_count: int):
        exception = make_exception()

        self._dll_handle.TemplatesIndex_reserveSearchMemory(
            self._impl,
            c_int64(queries_count),
            exception
        )

        check_exception(exception, self._dll_handle)