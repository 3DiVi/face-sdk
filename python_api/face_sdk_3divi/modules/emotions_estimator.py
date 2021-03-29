##
#  \file emotions_estimator.py
#  \~English
#     \brief EmotionsEstimator - Interface object for estimation of emotions.
#  \~Russian
#     \brief EmotionsEstimator - интерфейсный объект для определения эмоций.

from enum import Enum

from ctypes import c_int, c_void_p, c_float, py_object, sizeof, c_int32

from typing import List, Union
from io import BytesIO

from .wrap_funcs import write_func
from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from . import get_repr

## @defgroup PythonAPI
#  @{
## @defgroup EmotionsEstimator
#  @{

##
# \~English
#    \brief Emotions.
# \~Russian
#    \brief Эмоции.
class Emotion(Enum):

    ##
    #  \~English
    #     \brief Neutral.
    #  \~Russian
    #     \brief Нейтральный.
    EMOTION_NEUTRAL = 0

    ##
    #  \~English
    #     \brief Happy.
    #  \~Russian
    #     \brief Счастливый.
    EMOTION_HAPPY = 1

    ##
    #  \~English
    #     \brief Angry.
    #  \~Russian
    #     \brief Злой.
    EMOTION_ANGRY = 2

    ##
    #  \~English
    #     \brief Surprised.
    #  \~Russian
    #     \brief Удивленный.
    EMOTION_SURPRISE = 3


##
# \~English
#    \brief Emotion with confidence.
# \~Russian
#    \brief Эмоция с коэффициентом уверенности.
class EmotionConfidence:

    ##
    #  \~English
    #     \brief Emotion.
    #  \~Russian
    #     \brief Эмоция.
    emotion: Emotion

    ##
    #  \~English
    #     \brief Positive real number in the range of [0; 1].
    #  \~Russian
    #     \brief Положительное вещественное число из диапазона [0; 1].
    confidence: float

    def __init__(self, emotion: Union[int, Emotion], confidence: float):
        if isinstance(emotion, Emotion):
            self.emotion = emotion
        else:
            # for internal using
            self.emotion = Emotion(emotion)
        self.confidence = confidence

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Interface object for estimation of emotions.
# \~Russian
#    \brief Интерфейсный объект для определения эмоций.
class EmotionsEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(EmotionsEstimator, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Estimates the emotion of a given face sample.
    #
    #    \param[in] sample
    #      Face sample.
    #
    #    \return Vector of estimated emotions with confidence coefficients.
    #      In descending confidence order.
    #      The sum of all confidence coefficients equals 1.
    #
    # \~Russian
    #    \brief Определить эмоцию поданного лица.
    #
    #    \param[in] sample
    #      Лицо.
    #
    #    \return Определенные эмоции с коэффициентами уверенности.
    #      В порядке уменьшения уверенности.
    #      Сумма всех коэффициентов равна 1.
    def estimate_emotions(self, sample: RawSample) -> List[EmotionConfidence]:
        exception = make_exception()

        data_oss_w = BytesIO()

        count = self._dll_handle.EmotionsEstimator_estimateEmotions(
            self._impl,
            sample._impl,
            py_object(data_oss_w),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)

        data_oss_w.seek(0)
        result = list()
        int_size = sizeof(c_int32)
        float_size = sizeof(c_float)

        for i in range(count):
            emotion = c_int.from_buffer_copy(data_oss_w.read(int_size)).value
            confidence = c_float.from_buffer_copy(data_oss_w.read(float_size)).value
            emotion_confidence = EmotionConfidence(emotion, confidence)

            result.append(emotion_confidence)

        return result
