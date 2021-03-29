##
#  \file face_attributes_estimator.py
#  \~English
#     \brief FaceAttributesEstimator is an interface object used to estimate attributes of the face.
#  \~Russian
#     \brief FaceAttributesEstimator - интерфейсный объект для определения аттрибутов лица.

from enum import Enum

from .processing_block import ProcessingBlock
from .dll_handle import DllHandle
from .raw_sample import RawSample

## @defgroup PythonAPI
#  @{
## @defgroup FaceAttributesEstimator
#  @{


##
#  \~English
#     \brief Detailed description for attribute - medical mask.
#  \~Russian
#     \brief Детальное описание для атрибута - медицинская маска.
class MaskAttribute(Enum):
    NOT_COMPUTED = 0
    NO_MASK = 1
    HAS_MASK = 2


##
#  \~English
#     \brief Estimated eye state.
#  \~Russian
#     \brief Результат определения состояния глаза.
class EyeState(Enum):
    NOT_COMPUTED = 0
    CLOSED = 1
    OPENED = 2


class EyeStateScore:
    ##
    #  \~English
    #     \brief Estimated eye state.
    #  \~Russian
    #     \brief Результат определения состояния глаза.
    eye_state: EyeState

    ##
    #  \~English
    #     \brief Probability of eye opening.
    #  \~Russian
    #     \brief Вероятность, что глаз открыт.
    score: float

    def __init__(self):
        self.eye_state = EyeState.NOT_COMPUTED
        self.score = -1


##
#  \~English
#     \brief Interface object used to estimate attributes of the face.
#  \~Russian
#     \brief Интерфейсный объект для определения аттрибутов лица.
class Attribute:
    ##
    #  \~English
    #     \brief Probability of a required attribute.
    #  \~Russian
    #     \brief Вероятность наличия атрибута.
    score: float

    ##
    #  \~English
    #     \brief Detailed description for attribute - medical mask.
    #  \~Russian
    #     \brief Детальное описание для атрибута - медицинская маска.
    mask_attribute: MaskAttribute

    ##
    #  \~English
    #     \brief Estimated left eye state and probability of eye opening.
    #  \~Russian
    #     \brief Результат определения состояния левого и вероятность, что глаз открыт.
    left_eye_state: EyeStateScore

    ##
    #  \~English
    #     \brief Estimated right eye state and probability of eye opening.
    #  \~Russian
    #     \brief Результат определения состояния правого глаза и вероятность, что глаз открыт.
    right_eye_state: EyeStateScore

    def __init__(self):
        self.mask_attribute = MaskAttribute.NOT_COMPUTED
        self.left_eye_state = EyeStateScore()
        self.right_eye_state = EyeStateScore()
        self.score = -1


##
#  \~English
#     \brief Interface object used to estimate attributes of the face.
#  \~Russian
#     \brief Интерфейсный объект для определения аттрибутов лица.
class FaceAttributesEstimator(ProcessingBlock):

    def __init__(self, dll_handle: DllHandle, config: str):
        super(FaceAttributesEstimator, self).__init__(dll_handle, config)

    ##
    #  \~English
    #     \brief Estimates if there is is a required attribute on a given face sample.
    #
    #     \param[ in]  sample
    #       Face sample.
    #
    #     \return Estimated verdict and score(see Attribute for details)..
    #
    #  \~Russian
    #     \brief Оценивает, есть ли нужный аттрибут на данном сэмпле лица.
    #
    #     \param[ in]  sample
    #       Лицо.
    #
    #     \return Определенный результат с коэффициентами уверенности(см. Attribute).
    def estimate(self, raw_sample: RawSample) -> Attribute:
        ctx = {
            "raw_sample_ptr_str": str(raw_sample._impl.value),
        }
        new_ctx = self.process(ctx)

        result = Attribute()
        task_name = new_ctx["task_name"]

        if task_name == "masked_face":
            result.score = new_ctx["mask_score"]
            result.mask_attribute = MaskAttribute.HAS_MASK if new_ctx["mask_verdict"] else MaskAttribute.NO_MASK
        elif task_name == "eyes_openness":
            result.left_eye_state.eye_state = EyeState.OPENED if new_ctx["eye_openness_left_verdict"] else EyeState.CLOSED
            result.right_eye_state.eye_state = EyeState.OPENED if new_ctx["eye_openness_right_verdict"] else EyeState.CLOSED
            result.left_eye_state.score = new_ctx["eye_openness_left_score"]
            result.right_eye_state.score = new_ctx["eye_openness_right_score"]
        else:
            raise Exception(f"FaceAttributesEstimator: unknown name_task: {task_name}")

        return result

