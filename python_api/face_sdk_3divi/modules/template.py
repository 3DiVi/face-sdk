##
#  \file Template.py
#  \~English
#     \brief Template - Interface object for saving the face template.
#  \~Russian
#     \brief Template - Интерфейсный объект для сохранения шаблона лица.

from ctypes import c_void_p, py_object

from io import BytesIO

from .exception_check import check_exception, make_exception
from .wrap_funcs import write_func
from .complex_object import ComplexObject
from .dll_handle import DllHandle

## @defgroup PythonAPI
#  @{
## @defgroup Template
#  @{

##
# \~English
#    \brief Interface object for saving the face template.
# \~Russian
#    \brief Интерфейсный объект для сохранения шаблона лица.
class Template(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(Template, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Get a method name.
    #      Thread-safe.
    #
    #    \return The name of the method that created this template.
    #
    # \~Russian
    #    \brief Получить имя метода.
    #      Потокобезопасный.
    #
    #    \return Имя метода, создавшего этот шаблон.
    def get_method_name(self) -> str:
        name_stream_wrap = BytesIO()

        exception = make_exception()

        self._dll_handle.Template_getMethodName(
            self._impl,
            py_object(name_stream_wrap),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)

        name = name_stream_wrap.getvalue()
        name_stream_wrap.close()
        return name.decode()

    ##
    # \~English
    #    \brief Save the template to binary_stream.
    #      The format is platform-independent.
    #      Thread-safe.
    #
    #    \param[in] binary_stream
    #      Output stream object for saving the template.
    #      The file stream (std::ifstream) must be opened with the set std::ios_base::binary flag.
    #
    # \~Russian
    #    \brief Сохранить шаблон.
    #      Формат платформонезависимый.
    #      Потокобезопасный.
    #
    #    \param[in] binary_stream
    #      Объект потока.
    #      Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
    def save(self, binary_stream: BytesIO):
        exception = make_exception()

        self._dll_handle.Template_save(
            self._impl,
            py_object(binary_stream),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)
