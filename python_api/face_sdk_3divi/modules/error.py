##
#  \file error.py
#  \~English
#     \brief Error - the class of exceptions thrown when errors occur.
#  \~Russian
#     \brief Error - класс исключений, выбрасываемых при возникновении ошибок.


##
# \~English
#    \brief The class of exceptions thrown when errors occur.
# \~Russian
#    \brief Класс исключений, выбрасываемых при возникновении ошибок.
class Error(Exception):

    def __init__(self, code: hex, what: str):
        self._code = code
        self._what = what

    ##
    # \~English
    #    \brief Gets a string identifying an error.
    #
    #    \return A null-terminated character sequence identifying an error.
    #
    # \~Russian
    #    \brief Получить строку с описанием ошибки.
    #
    #    \return Нуль-терминированная строка с описанием ошибки.
    def what(self):
        return self._what

    ##
    # \~English
    #    \brief Gets an error code
    #
    #    \return An unsigned integer, which is an error code.
    #
    # \~Russian
    #    \brief Получить код ошибки.
    #
    #    \return Беззнаковое целое число - код ошибки.
    def code(self):
        if isinstance(self._code, int):
            return hex(self._code)
        else:
            return self._code

    def __str__(self):
        return "{}: {}".format(self.code(), self.what())
