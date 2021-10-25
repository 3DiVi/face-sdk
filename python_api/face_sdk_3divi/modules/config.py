##
#  \file config.py

from typing import List
from . import get_repr

## @defgroup PythonAPI
#  @{

##
#  \~English
#     \brief Used to override configuration parameters at runtime.
#     \param[in] config_filepath
#       Path to a configuration file
#
#  \~Russian
#     \brief Класс для переопределения параметров конфигурации во время выполнения.
#     \param[in] config_filepath
#       Путь до конфигурационного файла
class Config:

    def __init__(self, config_filepath):

        self.config_filepath = config_filepath
        self.overriden = {}

    ##
    #  \~English
    #     \brief Override the parameter value.
    #     \param[in] parameter
    #       Parameter name (tag name in the .xml config file).
    #     \param[in] value
    #       New parameter value.
    #
    #  \~Russian
    #     \brief Переопределить значение параметра.
    #     \param[in] parameter
    #       Имя параметра (имя тэга из .xml конфигурационного файла).
    #     \param[in] value
    #       Новое значение параметра.
    def override_parameter(self, parameter, value):
        self.overriden[parameter] = value
        return self

    def prepare(self) -> (List[str], List[float]):
        return list(self.overriden.keys()), list(self.overriden.values())

    def __repr__(self):
        return get_repr(self)
