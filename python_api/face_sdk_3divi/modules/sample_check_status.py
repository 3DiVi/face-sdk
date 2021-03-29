##
#  \file sample_check_status.py

from enum import Enum

## @defgroup PythonAPI
#  @{
## @defgroup Verdict
#  @{

##
# \~English
#    \brief The result of some kind of sample checking.
# \~Russian
#    \brief Результат некоторой проверки сэмпла.
class Verdict(Enum):

    ##
    #  \~English
    #     \brief Sample passed the checking.
    #  \~Russian
    #     \brief Сэмпл прошел проверку.
    PASSED = 0

    ##
    #  \~English
    #     \brief Sample did not pass the checking.
    #  \~Russian
    #     \brief Сэмпл не прошел проверку.
    FAILED = 1

    ##
    #  \~English
    #     \brief Sample was not checked.
    #  \~Russian
    #     \brief Сэмпл не проверялся.
    NOT_COMPUTED = 2
