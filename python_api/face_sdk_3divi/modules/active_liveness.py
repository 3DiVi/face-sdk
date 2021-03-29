## @defgroup PythonAPI
#  @{
#  \~English
#    \brief Python API interface.
#  \~Russian
#    \brief Python API интерфейс.


## @defgroup ActiveLiveness
#  @{
from enum import Enum


##
#  \~English
#    \brief Check type for active liveness.
#  \~Russian
#    \brief Тип проверки для активной (сценарной) оценки принадлежности лица живому человеку.
class CheckType(Enum):
    NONE = 0
    SMILE = 1
    BLINK = 2
    TURN_UP = 3
    TURN_DOWN = 4
    TURN_RIGHT = 5
    TURN_LEFT = 6
    PERSPECTIVE = 7


##
#  \~English
#    \brief Active liveness check state.
#  \~Russian
#    \brief Состояние проверки активной (сценарной) оценки принадлежности лица живому человеку.
class Liveness(Enum):
    ALL_CHECKS_PASSED = 0
    CURRENT_CHECK_PASSED = 1
    CHECK_FAIL = 2
    WAITING_FACE_ALIGN = 3
    IN_PROGRESS = 4
    NOT_COMPUTED = 5


##
#  \~English
#    \brief Active liveness check state description.
#  \~Russian
#    \brief Объект для полного описания состояния активной (сценарной) оценки принадлежности лица живому человеку.
class ActiveLivenessStatus:
    verdict = Liveness.NOT_COMPUTED

    check_type = CheckType.NONE

    progress_level = 0.0

    def __init__(self, verdict, check_type, progress_level):
        self.verdict = verdict
        self.check_type = check_type
        self.progress_level = progress_level
