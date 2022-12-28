##
#  \file VideoWorker.py
#  \~English
#     \brief VideoWorker is an interface object for tracking, processing and matching faces on multiple video streams.
#  \~Russian
#     \brief VideoWorker - интерфейсный объект для трекинга, обработки и распознавания лиц на нескольких видеопотоках.

from ctypes import CFUNCTYPE, c_double
from ctypes import c_int32, c_void_p, py_object, c_float, sizeof, create_string_buffer, c_char_p, \
    POINTER, c_int64, c_uint64
from enum import IntEnum

from typing import List, Union
from io import BytesIO

from .exception_check import check_exception, make_exception
from .recognizer import MatchResult, SearchAccelerationType
from .struct_storage_fields import StructStorageFields
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .config import Config
from .age_gender_estimator import Age, Gender, AgeGender
from .emotions_estimator import EmotionConfidence
from .raw_image import RawImage
from .depth_map_raw import DepthMapRaw
from . import depth_liveness_estimator
from .ir_frame_raw import IRFrameRaw
from . import ir_liveness_estimator
from .sample_check_status import Verdict
from .raw_sample import RawSample
from .template import Template
from .error import Error
from . import get_repr
from .wrap_funcs import write_func
from . import active_liveness


## @defgroup PythonAPI
#  @{
## @defgroup VideoWorker
#  @{

##
# \~English
#    \brief Template search result (for  face_sdk_3divi.modules.video_worker.MatchFoundCallbackFuncU).
# \~Russian
#    \brief Результат поиска шаблона (для face_sdk_3divi.modules.video_worker.MatchFoundCallbackFuncU).
class SearchResult:
    ##
    #  \~English
    #     \brief Unique id of the person.
    #  \~Russian
    #     \brief Уникальный идентификатор человека.
    person_id: int

    ##
    #  \~English
    #     \brief Unique id of the element.
    #  \~Russian
    #     \brief Уникальный идентификатор элемента.
    element_id: int

    ##
    #  \~English
    #     \brief Result of template matching.
    #  \~Russian
    #     \brief Результат сравнения шаблонов.
    match_result: MatchResult

    ##
    def __init__(self, person_id: int, element_id: int, match_result: MatchResult):
        self.person_id = person_id
        self.element_id = element_id
        self.match_result = match_result

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Tracking callback data.
# \~Russian
#    \brief Данные Tracking коллбэка.
class TrackingCallbackData:
    ##
    #  \~English
    #     \brief Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #  \~Russian
    #     \brief Целочисленный идентификатор видеопотока
    #     (0 <= stream_id < streams_count).
    stream_id: int

    ##
    #  \~English
    #     \brief Integer id of the frame
    #     (that was returned by VideoWorker.add_video_frame).
    #  \~Russian
    #     \brief Целочисленный идентификатор кадра
    #     (который был возвращен методом VideoWorker.add_video_frame).
    frame_id: int

    ##
    #  \~English
    #     \brief Vector of face samples found by the tracker.
    #     Most of the samples are received from the frame_id frame
    #     but some samples can be received from the previous frames.
    #     Use RawSample.get_frame_id to determine
    #     which frame the sample belongs to.
    #  \~Russian
    #     \brief Вектор лиц, найденных трекером.
    #     Большинство сэмплов будут получены с кадра frame_id,
    #     но некоторые могут быть получены с предыдущих кадров.
    #     Используйте метод RawSample.get_frame_id,
    #     чтобы определить, к какому кадру относится сэмпл.
    samples: List[RawSample]

    ##
    #  \~English
    #     \brief Vector of face IDs (track_id).
    #     track_id is equal to sample.get_id() for the sample given in any VideoWorker callback.
    #     (samples_track_id.size() == samples.size())
    #  \~Russian
    #     \brief Вектор идентификаторов лиц (track_id).
    #     track_id равен sample.get_id() для сэмпла в любом VideoWorker коллбэке.
    #     (samples_track_id.size() == samples.size())
    samples_track_id: List[int]

    ##
    #  \~English
    #     \brief Since this is tracking, some samples may be false,
    #     so we mark the samples with the "weak" flag
    #     if they haven’t passed the recheck with the frontal face detector.
    #     Weak samples are not used for recognition
    #     (samples_weak.size() == samples.size())
    #  \~Russian
    #     \brief Так как это трекинг, то некоторые сэмплы могут быть ошибочными,
    #     поэтому мы помечаем сэмплы флагом "weak", если они не прошли
    #     повторную проверку фронтальным детектором лиц.
    #     "weak" сэмплы не используются для распознавания
    #     (samples_weak.size() == samples.size())
    samples_weak: List[bool]

    ##
    #  \~English
    #     \brief Quality value for the face sample.
    #     The same as from the FaceQualityEstimator
    #     (samples_quality.size() == samples.size())
    #  \~Russian
    #     \brief Качество сэмпла.
    #     Точно такое же, как и у FaceQualityEstimator
    #     (samples_quality.size() == samples.size())
    samples_quality: List[float]

    ##
    #  \~English
    #     \brief The result of checking the sample for good lighting conditions and absence of a strong blur
    #     (samples_good_light_and_blur.size() == samples.size())
    #  \~Russian
    #     \brief Результат проверки сэмпла на хорошие условия освещения и отсутствие сильного размытия
    #     (samples_good_light_and_blur.size() == samples.size())
    samples_good_light_and_blur: List[Verdict]

    ##
    #  \~English
    #     \brief The result of checking the sample for absence of too high yaw/pitch angles
    #     (samples_good_angles.size() == samples.size())
    #  \~Russian
    #     \brief Результат проверки сэмпла на отсутствие слишком больших углов поворота лица
    #     (samples_good_angles.size() == samples.size())
    samples_good_angles: List[Verdict]

    ##
    #  \~English
    #     \brief The result of checking the sample for suitable size of a face,
    #     see the <i>min_template_generation_face_size</i> parameter in the configuration file.
    #     len(samples_good_face_size) == len(samples)
    #  \~Russian
    #     \brief Результат проверки сэмпла на приемлемый размер лица,
    #     см. параметр <i>min_template_generation_face_size</i> в конфигурационном файле.
    #     len(samples_good_face_size) == len(samples)
    samples_good_face_size: List[Verdict]

    ##
    #  \~English
    #     \brief The result of checking the sample with the frontal face detector
    #     len(samples_detector_confirmed) == len(samples.size()
    #  \~Russian
    #     \brief Результат проверки сэмпла фронтальным детектором лиц
    #     len(samples_detector_confirmed) == len(samples.size()
    samples_detector_confirmed: List[Verdict]

    ##
    #  \~English
    #     \brief The result of checking the sample with DepthLivenessEstimator,
    #     depth frames are required, see VideoWorker.addDepthFrame.
    #     See face_sdk_3divi.modules.depth_liveness_estimator.Liveness for details.
    #     len(samples_depth_liveness_confirmed) == len(samples)
    #  \~Russian
    #     \brief Результат проверки сэмпла посредством DepthLivenessEstimator,
    #     требуются кадры глубины, см. VideoWorker.addDepthFrame.
    #     См. также face_sdk_3divi.modules.depth_liveness_estimator.Liveness.
    #     len(samples_depth_liveness_confirmed) == len(samples)
    samples_depth_liveness_confirmed: List[depth_liveness_estimator.Liveness]

    ##
    #  \~English
    #     \brief The result of checking the sample with IRLivenessEstimator,
    #     depth frames are required, see VideoWorker.addIRFrame.
    #     See face_sdk_3divi.modules.ir_liveness_estimator.Liveness for details.
    #     len(samples_ir_liveness_confirmed) == samples.size)
    #  \~Russian
    #     \brief Результат проверки сэмпла посредством IRLivenessEstimator,
    #     требуются кадры глубины, см. VideoWorker.addIRFrame.
    #     См. также face_sdk_3divi.modules.ir_liveness_estimator.Liveness.
    #     len(samples_ir_liveness_confirmed) == samples.size)
    samples_ir_liveness_confirmed: List[ir_liveness_estimator.Liveness]

    ##
    #  \~English
    #     \brief Flag indicating that age and gender were estimated for this track.
    #     len(samples_track_age_gender_set) == len(samples)
    #  \~Russian
    #     \brief Флаг, обозначающий, что пол и возраст определены для этого трека.
    #     len(samples_track_age_gender_set) == len(samples)
    samples_track_age_gender_set: List[bool]

    ##
    #  \~English
    #     \brief Estimated age and gender for this track.
    #     len(samples_track_age_gender) == len(samples)
    #  \~Russian
    #     \brief Оценки пола и возраста для этого трека.
    #     len(samples_track_age_gender) == len(samples)
    samples_track_age_gender: List[AgeGender]

    ##
    #  \~English
    #     \brief Flag indicating that emotions were estimated for this track.
    #     len(samples_track_emotions_set) == len(samples)
    #  \~Russian
    #     \brief Флаг, обозначающий, что эмоции определены для этого трека.
    #     len(samples_track_emotions_set) == len(samples)
    samples_track_emotions_set: List[bool]

    ##
    #  \~English
    #     \brief Estimated emotions for this track.
    #     len(samples_track_emotions) == len(samples)
    #  \~Russian
    #     \brief Оценка эмоций для этого трека.
    #     len(samples_track_emotions) == len(samples)
    samples_track_emotions: List[EmotionConfidence]

    ##
    #  \~English
    #     \brief Face active liveness check status.
    #     See ActiveLiveness::ActiveLivenessStatus for details.
    #     (samples_active_liveness_status.size() == samples.size())
    #  \~Russian
    #     \brief Состояние проверки лица на принадлежность живому человеку
    #     посредством сценария.
    #     См. также ActiveLiveness::ActiveLivenessStatus.
    #     (samples_active_liveness_status.size() == samples.size())
    samples_active_liveness_status: List[active_liveness.ActiveLivenessStatus]

    ##
    def __init__(self):
        self.stream_id = 0
        self.frame_id = 0
        self.samples: List[RawSample] = list()
        self.samples_track_id: List[int] = list()
        self.samples_weak: List[bool] = list()
        self.samples_quality: List[float] = list()
        self.samples_good_light_and_blur: List[Verdict] = list()
        self.samples_good_angles: List[Verdict] = list()
        self.samples_good_face_size: List[Verdict] = list()
        self.samples_detector_confirmed: List[Verdict] = list()
        self.samples_depth_liveness_confirmed: List[depth_liveness_estimator.Liveness] = list()
        self.samples_ir_liveness_confirmed: List[ir_liveness_estimator.Liveness] = list()
        self.samples_track_age_gender_set: List[bool] = list()
        self.samples_track_age_gender: List[AgeGender] = list()
        self.samples_track_emotions_set: List[bool] = list()
        self.samples_track_emotions: List[EmotionConfidence] = list()
        self.samples_active_liveness_status: List[active_liveness.ActiveLivenessStatus] = list()

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief TemplateCreated callback data.
# \~Russian
#    \brief Данные TemplateCreated коллбэка.
class TemplateCreatedCallbackData:
    ##
    #  \~English
    #     \brief Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #  \~Russian
    #     \brief Целочисленный идентификатор видеопотока
    #     (0 <= stream_id < streams_count).
    stream_id: int

    ##
    #  \~English
    #     \brief Integer id of the frame
    #     (that was returned by VideoWorker.add_video_frame).
    #  \~Russian
    #     \brief Целочисленный идентификатор кадра
    #     (который был возвращен методом VideoWorker.add_video_frame).
    frame_id: int

    ##
    #  \~English
    #     \brief Face sample quality.
    #     The same as from the FaceQualityEstimator.
    #  \~Russian
    #     \brief Качество сэмпла.
    #     Точно такое же, как и у FaceQualityEstimator.
    quality: float

    ##
    #  \~English
    #     \brief Face sample.
    #  \~Russian
    #     \brief Сэмпл лица.
    sample: RawSample

    ##
    #  \~English
    #     \brief Template created for this sample.
    #  \~Russian
    #     \brief Шаблон, созданный из sample.
    templ: Template

    ##
    def __init__(self, stream_id: int, frame_id: int, quality: float, sample: RawSample, templ: Template):
        self.stream_id = stream_id
        self.frame_id = frame_id
        self.quality = quality
        self.sample = sample
        self.templ = templ

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief MatchFound callback data.
# \~Russian
#    \brief Данные MatchFound коллбэка.
class MatchFoundCallbackData(TemplateCreatedCallbackData):
    ##
    #  \~English
    #     \brief Found elements with a distance less than distance_threshold
    #     (but not more than <i>k</i> nearest elements, the value of <i>k</i> is set
    #     in the configuration file tagged as <i>search_k</i> tag).
    #
    #     When called after N consecutive mismatches,
    #     the first element in search_results will have a zero distance,
    #     and person_id and element_id are equal to face_sdk_3divi.modules.video_worker.MATCH_NOT_FOUND_ID.
    #  \~Russian
    #     \brief Найденные элементы с расстоянием меньше distance_threshold
    #     (но не более k ближайших элементов; величина k установлена
    #     в конфигурационном файле под тегом search_k).
    #
    #     При вызове после N последовательных несовпадений первый элемент в search_results
    #     будет иметь нулевое расстояние, а person_id и element_id равны
    #     face_sdk_3divi.modules.video_worker.MATCH_NOT_FOUND_ID.
    search_results: List[SearchResult]

    ##
    def __init__(self, stream_id: int, frame_id: int, quality: float, sample: RawSample, templ: Template,
                 search_results: List[SearchResult]):
        super(MatchFoundCallbackData, self).__init__(stream_id, frame_id, quality, sample, templ)
        self.search_results = search_results

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief TrackingLost callback data.
# \~Russian
#    \brief Данные TrackingLost коллбэка.
class TrackingLostCallbackData:
    ##
    #  \~English
    #     \brief Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #  \~Russian
    #     \brief Целочисленный идентификатор видеопотока
    #     (0 <= stream_id < streams_count).
    stream_id: int

    ##
    #  \~English
    #     \brief Integer id of the frame on which the face was first detected.
    #  \~Russian
    #     \brief Целочисленный идентификатор кадра, на котором лицо было замечено впервые.
    first_frame_id: int

    ##
    #  \~English
    #     \brief Integer id of the frame after which the face tracking was lost.
    #  \~Russian
    #     \brief Целочисленный идентификатор кадра, после которого трекинг лица был потерян.
    last_frame_id: int

    ##
    #  \~English
    #     \brief Best sample quality over all frames.
    #  \~Russian
    #     \brief Лучшее качество среди всех кадров.
    best_quality: float

    ##
    #  \~English
    #     \brief Integer id of the frame of the best quality.
    #  \~Russian
    #     \brief Целочисленный идентификатор кадра с наилучшим качеством.
    best_quality_frame_id: int

    ##
    #  \~English
    #     \brief ID of the lost face (track_id).
    #     track_id is equal to sample.get_id() for a sample given in any VideoWorker callback.
    #  \~Russian
    #     \brief Идентификатор потерянного лица (track_id).
    #     track_id равен sample.get_id() для сэмпла в любом VideoWorker коллбэке.
    track_id: int

    ##
    #  \~English
    #     \brief The best quality sample from the best_quality_frame_id frame.
    #     Will be NULL if "weak_tracks_in_tracking_callback" is enabled and all samples with that track_id
    #     are flagged as "weak=true".
    #  \~Russian
    #     \brief Лучший сэмпл лица с кадра best_quality_frame_id.
    #       Будет равен NULL, если включен флаг "weak_tracks_in_tracking_callback" и все сэмплы с данным track_id
    #       помечены флагом "weak=true".
    best_quality_sample: Union[RawSample, None]

    ##
    #  \~English
    #     \brief Template created from best_quality_sample.
    #     Will be NULL if processing_threads_count is zero or best_quality_sample is NULL.
    #  \~Russian
    #     \brief Шаблон, созданный из best_quality_sample.
    #     Будет равен NULL при нулевом количестве потоков обработки или best_quality_sample, равном NULL.
    best_quality_templ: Union[Template, None]

    ##
    #  \~English
    #     \brief Flag indicating that sti_person_id is set. sti_person_id is not set, if short time identification is disabled
    #     or no templates were generated for this track.
    #  \~Russian
    #     \brief Флаг, означающий, что sti_person_id определен. Значение sti_person_id не определено, если кратковременная идентификация отключена,
    #     либо если для данного трека не было сгенерировано ни одного шаблона.
    sti_person_id_set: bool

    ##
    #  \~English
    #     \brief ID of "sti_person", which is a set of tracks formed by short time identification.
    #     sti_person_id is equal to track_id of the first member of this "sti_person" set of tracks.
    #  \~Russian
    #     \brief Идентификатор "sti_person" - набора треков, объединенных кратковременной идентификацией.
    #     Значение sti_person_id совпадает с track_id первого элемента, сформировавшего этот набор "sti_person".
    sti_person_id: int

    ##
    def __init__(self):
        self.stream_id = 0
        self.first_frame_id = 0
        self.last_frame_id = 0
        self.best_quality = 0
        self.best_quality_frame_id = 0
        self.track_id = 0
        self.best_quality_sample = None
        self.best_quality_templ = None
        self.sti_person_id_set = False
        self.sti_person_id = 0

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief StiPersonOutdated callback data.
# \~Russian
#    \brief Данные StiPersonOutdated коллбэка.
class StiPersonOutdatedCallbackData:
    ##
    #  \~English
    #     \brief Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #  \~Russian
    #     \brief Целочисленный идентификатор видеопотока
    #     (0 <= stream_id < streams_count).
    stream_id: int

    ##
    #  \~English
    #     \brief ID of "sti_person", which is a set of tracks formed by short time identification.
    #     sti_person_id is equal to track_id of the first mebmer of this "sti_person" set of tracks.
    #  \~Russian
    #     \brief Идентификатор "sti_person" - набора треков, объединенных кратковременной идентификацией.
    #     Значение sti_person_id совпадает с track_id первого элемента, сформировавшего этот набор "sti_person".
    sti_person_id: int

    ##
    def __init__(self, stream_id, sti_person_id):
        self.stream_id = stream_id
        self.sti_person_id = sti_person_id

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Database element.
#      If you have several face templates for one person,
#      you should create one element for each template
#      with the same person_id but different element_id.
#
# \~Russian
#     \brief Элемент базы.
#       Если имеется несколько шаблонов для одного человека,
#       нужно создать по одному элементу для каждого шаблона
#       с одинаковыми person_id, но разными element_id.
class DatabaseElement:
    ##
    #  \~English
    #     \brief Unique id of the element.
    #  \~Russian
    #     \brief Уникальный идентификатор элемента.
    element_id: int

    ##
    #  \~English
    #     \brief Unique id of the person.
    #  \~Russian
    #     \brief Уникальный идентификатор человека.
    person_id: int

    ##
    #  \~English
    #     \brief Face template.
    #  \~Russian
    #     \brief Шаблон лица.
    face_template: Template

    ##
    #  \~English
    #     \brief Distance threshold for a MatchFound event.
    #
    #     Setting a different recognition threshold for different persons may be useful in some cases.
    #     For example, if there is a particular criminal in the blacklist, who is more dangerous
    #     than the other criminals, and you want to increase the recognition probability for this very person,
    #     despite the higher probability of a false acceptance error.
    #
    #  \~Russian
    #     \brief Порог распознавания.
    #
    #     Установка разных порогов распознавания для разных людей может быть полезной в некоторых случаях.
    #     Например, в том случае, если определенный преступник из черного списка опаснее остальных,
    #     и вы хотите увеличить вероятность его распознавания,
    #     несмотря на увеличение вероятности ложного распознавания.
    distance_threshold: float

    ##
    def __init__(self, element_id: int, person_id: int, face_template: Template, distance_threshold: float):
        self.element_id = element_id
        self.person_id = person_id
        self.face_template = face_template
        self.distance_threshold = distance_threshold

    def __repr__(self):
        return get_repr(self)


MATCH_NOT_FOUND_ID = -1


##
# \~English
#    \brief Parameters of the VideoWorker constructor.
# \~Russian
#    \brief Параметры конструктора VideoWorker.
class Params:
    ##
    #  \~English
    #     \brief Set the VideoWorker configuration file with optionally overridden parameters.
    #  \~Russian
    #     \brief Задать конфигурационный файл VideoWorker с опционально переопределенными параметрами.
    video_worker_config: Union[Config, None]

    ##
    #  \~English
    #     \brief Set the name of the configuration file for Recognizer that will be used inside VideoWorker.
    #  \~Russian
    #     \brief Задать имя конфигурационного файла для используемого распознавателя (Recognizer).
    recognizer_ini_file: str

    ##
    #  \~English
    #     \brief Set the configuration file with optionally overridden parameters for Recognizer that will be used inside VideoWorker.
    #  \~Russian
    #     \brief Задать конфигурационный файл Recognizer с опционально переопределенными параметрами (Recognizer).
    recognizer_config: Config

    ##
    #  \~English
    #     \brief Set the number of video streams.
    #  \~Russian
    #     \brief Задать количество видеопотоков.
    streams_count: int

    ##
    #  \~English
    #     \brief Set the number of threads for creating templates.
    #  \~Russian
    #     \brief Задать количество потоков для создания шаблонов.
    processing_threads_count: int

    ##
    #  \~English
    #     \brief Set the number of threads for matching templates with the database.
    #  \~Russian
    #     \brief Задать количество потоков для сравнения шаблонов, созданных из видеопотоков, с базой.
    matching_threads_count: int

    ##
    #  \~English
    #     \brief Set the number of threads for age_gender estimation.
    #  \~Russian
    #     \brief Задать количество потоков для определения пола и возраста.
    age_gender_estimation_threads_count: int

    ##
    #  \~English
    #     \brief Set the number of threads for estimation of emotions.
    #  \~Russian
    #     \brief Задать количество потоков для определения эмоций.
    emotions_estimation_threads_count: int

    ##
    #  \~English
    #     \brief Set the flag enabling short time #identification.
    #  \~Russian
    #     \brief Задать флаг, включающий кратковременную идентификацию.
    short_time_identification_enabled: bool

    ##
    # \~English
    #    \brief Set the recognition distance threshold for short time identification.
    # \~Russian
    #    \brief Задать порог распознавания для кратковременной идентификации.
    short_time_identification_distance_threshold: float

    ##
    #  \~English
    #     \brief Set outdate_time in seconds for short time #identification.
    #  \~Russian
    #     \brief Задать в секундах длину временного интервала для кратковременной идентификации.
    short_time_identification_outdate_time_seconds: float

    ##
    #  \~English
    #     \brief Set outdate_time in seconds for short time #identification.
    #  \~Russian
    #     \brief Задать в секундах длину временного интервала для кратковременной идентификации.
    active_liveness_checks_order: List[active_liveness.CheckType]

    ##
    #  \~English
    #     \brief \n Default constructor, set strings to empty, numbers to zero, flags to false.
    #  \~Russian
    #     \brief \n Конструктор по умолчанию, устанавливает строки пустыми, числа нулем, флаги выключены.
    def __init__(self):
        self.video_worker_config = None
        self.recognizer_ini_file = ""
        self.recognizer_config = Config("")
        self.streams_count = 0
        self.processing_threads_count = 0
        self.matching_threads_count = 0
        self.age_gender_estimation_threads_count = 0
        self.emotions_estimation_threads_count = 0
        self.short_time_identification_enabled = False
        self.short_time_identification_distance_threshold = 0
        self.short_time_identification_outdate_time_seconds = 0
        self.active_liveness_checks_order = list()

    def __repr__(self):
        return get_repr(self)


class VideoWorkerSomething(IntEnum):
    Processing_Enable = 1
    Processing_Disable = 2
    AgeGender_Enable = 3
    AgeGender_Disable = 4
    Emotions_Enable = 5
    Emotions_Disable = 6


##
# \~English
#    \brief VideoWorker is an interface object for tracking, processing and matching faces on multiple video streams.
# \~Russian
#    \brief VideoWorker - интерфейсный объект для трекинга, обработки
#      и распознавания лиц на нескольких видеопотоках.
class VideoWorker(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(VideoWorker, self).__init__(dll_handle, impl)

        exception = make_exception()

        self._dll_handle.VideoWorker_setThisVW(
            self._impl,
            py_object(self),
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Add a new video frame for a specific video stream.
    #      Tracking and recognition results will be returned asynchronously
    #      via Tracking, MatchFound and TrackingLost callbacks.
    #      Thread-safe.
    #
    #    \param[in] frame
    #      Video frame.
    #      Only colored images are allowed
    #      (i.e. face_sdk_3divi.modules.raw_image.Format.FORMAT_RGB, face_sdk_3divi.modules.raw_image.Format.FORMAT_BGR,
    #      face_sdk_3divi.modules.raw_image.format.FORMAT_YUV_NV21 and face_sdk_3divi.modules.raw_image.Format.FORMAT_YUV_NV12).
    #
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #      Timestamp of the frame in microseconds
    #
    #    \return Integer id for this frame, unique for this video stream.
    #      This id will be used to identify this frame in the callbacks.
    #
    # \~Russian
    #    \brief Подать новый кадр определенного видеопотока.
    #      Результаты трекинга и распознавания будут возвращаться асинхронно
    #      в коллбэках Tracking, MatchFound и TrackingLost.
    #      Потокобезопасный.
    #
    #    \param[in] frame
    #      Изображение кадра.
    #      Допустимы только цветные изображения
    #      (т.е. face_sdk_3divi.modules.raw_image.Format.FORMAT_RGB, face_sdk_3divi.modules.raw_image.Format.FORMAT_BGR,
    #      face_sdk_3divi.modules.raw_image.Format.FORMAT_YUV_NV21 и face_sdk_3divi.modules.raw_image.Format.FORMAT_YUV_NV12).
    #
    #    \param[in] stream_id
    #       Целочисленный идентификатор видеопотока
    #       (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #       Временная метка кадра в микросекундах
    #
    #    \return Целочисленный идентификатор кадра, уникальный для этого видеопотока,
    #      который будет использоваться в коллбэках для обозначения этого кадра.
    def add_video_frame(self, frame: RawImage,
                        stream_id: int = -1,
                        timestamp_microsec: int = -1) -> int:
        exception = make_exception()

        cdata = frame.make_c_api_data()

        result = self._dll_handle.VideoWorker_addVideoFrameWithTimestamp_with_crop(
            self._impl,

            cdata.data,
            cdata.width,
            cdata.height,
            cdata.format.value,

            cdata.with_crop,
            cdata.crop_info_offset_x,
            cdata.crop_info_offset_y,
            cdata.crop_info_data_image_width,
            cdata.crop_info_data_image_height,

            c_int32(stream_id),
            c_uint64(timestamp_microsec),
            exception)

        check_exception(exception, self._dll_handle)
        return result

    ##
    # \~English
    #    \brief Add a new depth frame for a specific stream.
    #      Thread-safe.
    #
    #    \param[in] depth_frame
    #      Depth frame (DepthMapRaw) that contains depth data.
    #
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #      Timestamp of the frame in microseconds
    #
    # \~Russian
    #    \brief Подать кадр определенного видеопотока c данными глубины.
    #      Потокобезопасный.
    #
    #    \param[in] depth_frame
    #      Кадр глубины (DepthMapRaw).
    #
    #    \param[in] stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #      Временная метка кадра в микросекундах
    def add_depth_frame(self, depth_frame: DepthMapRaw,
                        stream_id: int,
                        timestamp_microsec: int):
        exception = make_exception()

        self._dll_handle.VideoWorker_addDepthFrame(
            self._impl,
            c_int32(depth_frame.depth_map_rows),
            c_int32(depth_frame.depth_map_cols),
            c_float(depth_frame.depth_map_2_image_offset_x),
            c_float(depth_frame.depth_map_2_image_offset_y),
            c_float(depth_frame.depth_map_2_image_scale_x),
            c_float(depth_frame.depth_map_2_image_scale_y),
            c_float(depth_frame.horizontal_fov),
            c_float(depth_frame.vertical_fov),
            c_float(depth_frame.depth_unit_in_millimeters),
            c_char_p(depth_frame.depth_data),
            c_int32(depth_frame.depth_data_stride_in_bytes),
            c_int32(stream_id),
            c_int32(timestamp_microsec),
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Add a new IR frame for a specific stream.
    #      Thread-safe.
    #
    #    \param[in] ir_frame
    #      IR frame (IRFrameRaw) that contains IR data.
    #
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #      Timestamp of a frame in microseconds.
    #
    # \~Russian
    #    \brief Подать кадр определенного видеопотока c инфракрасными данными.
    #      Потокобезопасный.
    #
    #    \param[in] ir_frame
    #      Инфракрасный кадр (IRFrameRaw).
    #
    #    \param[in] stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    #
    #    \param[in] timestamp_microsec
    #      Временная метка кадра в микросекундах
    def add_ir_frame(self, ir_frame: IRFrameRaw,
                     stream_id: int,
                     timestamp_microsec: int):
        exception = make_exception()

        self._dll_handle.VideoWorker_addIRFrame(
            self._impl,
            c_int32(ir_frame.ir_frame_rows),
            c_int32(ir_frame.ir_frame_cols),
            c_float(ir_frame.ir_frame_2_image_offset_x),
            c_float(ir_frame.ir_frame_2_image_offset_y),
            c_float(ir_frame.ir_frame_2_image_scale_x),
            c_float(ir_frame.ir_frame_2_image_scale_y),
            c_float(ir_frame.ir_horizontal_fov),
            c_float(ir_frame.ir_vertical_fov),
            c_char_p(ir_frame.ir_frame_data),
            c_float(ir_frame.ir_data_stride_in_bytes),
            c_int32(stream_id),
            c_int32(timestamp_microsec),
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Reset tracker state for a specified video stream.
    #      Thread-safe.
    #
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Сбросить состояние трекера для указанного видеопотока.
    #      Потокобезопасный.
    #
    #    \param[in] stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def reset_tracker_on_stream(self, stream_id: int):
        exception = make_exception()

        self._dll_handle.VideoWorker_resetTrackerOnStream(
            self._impl,
            stream_id,
            exception
        )

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Reset work state for the stream.
    #      It waits for the callbacks, which are currently being executed, therefore, do not synchronize
    #      the resetStream call with the code inside the callbacks, otherwise it can cause a deadlock.
    #      After resetStream is executed, no callbacks related to the previous frames or tracks will be called.
    #      <b>It is necessary</b> to release the memory allocated for all tracks with track_id less than the return
    #      value, since <b>there will be no TrackingLost callback</b> for the tracks removed during this reset.
    #      Thread-safe.
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #    \return Integer track_id_threshold, which means that all tracks that were
    #      removed during resetStream had track_id < track_id_threshold,
    #      and all new tracks will have track_id >= track_id_threshold.
    # \~Russian
    #    \brief Сбросить состояние работы для указанного видеопотока.
    #      В процессе происходит ожидание выполняющихся в данный момент коллбэков, поэтому
    #      не синхронизируйте вызов resetStream с кодом внутри коллбэков, иначе возможен deadlock.
    #      После возврата управления от resetStream не будет вызвано ни одного
    #      коллбэка, относящегося к предыдущим кадрам или трекам.
    #      <b>Необходимо</b> освободить память, выделенную для треков, track_id которых меньше возвращенного
    #      значения, так как для треков, удаленных в процессе этого сброса, <b>TrackingLost коллбэк вызван не будет</b>.
    #      Потокобезопасный.
    #    \param[in] stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    #    \return Целое число track_id_threshold, означающее, что все удаленные в
    #      ходе resetStream треки имели track_id < track_id_threshold,
    #      а все новые треки будут иметь track_id >= track_id_threshold.
    def reset_stream(self, stream_id: int):
        exception = make_exception()

        threshold_track_id = self._dll_handle.VideoWorker_resetStream(
            self._impl,
            stream_id,
            exception
        )

        check_exception(exception, self._dll_handle)

        return threshold_track_id

    ##
    # \~English
    #    \brief Exceptions from the callbacks and inner threads
    #      are rethrown when this method is called.
    #      Thread-safe.
    # \~Russian
    #    \brief Исключения, выброшенные в коллбэках и внутренних потоках,
    #      будут выброшены повторно при вызове данного метода.
    #      Потокобезопасный.
    def check_exception(self):
        exception = make_exception()

        self._dll_handle.VideoWorker_checkExceptions(
            self._impl,
            exception
        )

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Add a handler for a Tracking event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_tracking_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события трекинга (Tracking).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #    \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_tracking_callback, чтобы отписаться.
    def add_tracking_callback_u(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addTrackingCallbackU(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Add a handler for a TemplateCreated event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_template_created_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события создания шаблона (TemplateCreated).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #    \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_template_created_callback, чтобы отписаться.
    def add_template_created_callback_u(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addTemplateCreatedCallbackU(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Add a handler for a MatchFound event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_match_found_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события распознавания (MatchFound).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #   \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_match_found_callback, чтобы отписаться.
    def add_match_found_callback_u(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addMatchFoundCallbackU(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Add a handler for a MatchFound event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_match_found_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события распознавания (MatchFound).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #   \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_match_found_callback, чтобы отписаться.
    def add_match_found_callback_ext(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addMatchFoundCallbackExt(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Add a handler for a MatchFound event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_match_found_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события распознавания (MatchFound).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #   \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_match_found_callback, чтобы отписаться.
    def add_tracking_lost_callback_u(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addTrackingLostCallbackU(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Add a handler for a MatchFound event.
    #      Thread-safe.
    #    \param[in] callback
    #      Callback function.
    #    \param[in] userdata
    #      Any pointer.
    #      It will be passed to the callback as a userdata argument.
    #    \return Integer id for this callback.
    #      Use it for passing to VideoWorker.remove_sti_person_outdated_callback to unsubscribe.
    #
    # \~Russian
    #    \brief Добавить обработчик события распознавания (MatchFound).
    #      Потокобезопасный.
    #    \param[in] callback
    #      Функция коллбэка.
    #    \param[in] userdata
    #      Любой указатель.
    #      При вызове коллбэка он будет передан через аргумент userdata.
    #   \return Целочисленный идентификатор коллбэка.
    #      Используйте его для передачи в VideoWorker.remove_sti_person_outdated_callback, чтобы отписаться.
    def add_sti_person_outdated_callback_u(self, callback, userdata) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_addStiPersonOutdatedCallbackU(
            self._impl,
            py_object(callback),
            py_object(userdata),
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Remove a handler for Tracking event.
    #      Thread-safe.
    #    \param[in] callback_id
    #      Integer callback id returned by VideoWorker.add_tracking_callback.
    #
    # \~Russian
    #    \brief Удалить обработчик события создания шаблона (Tracking).
    #      Потокобезопасный.
    #    \param[in] callback_id
    #      Идентификатор коллбэка, полученный от VideoWorker.add_tracking_callback.
    def remove_tracking_callback(self, callback_id):
        exception = make_exception()

        self._dll_handle.VideoWorker_removeTrackingCallback(
            self._impl,
            callback_id,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Remove a handler for TrackingLost event.
    #      Thread-safe.
    #    \param[in] callback_id
    #      Integer callback id returned by VideoWorker.add_tracking_lost_callback.
    #
    # \~Russian
    #    \brief Удалить обработчик события создания шаблона (TrackingLost).
    #      Потокобезопасный.
    #    \param[in] callback_id
    #      Идентификатор коллбэка, полученный от VideoWorker.add_tracking_lost_callback.
    def remove_tracking_lost_callback(self, callback_id):
        exception = make_exception()

        self._dll_handle.VideoWorker_removeTrackingLostCallback(
            self._impl,
            callback_id,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Remove a handler for TemplateCreated event.
    #      Thread-safe.
    #    \param[in] callback_id
    #      Integer callback id returned by VideoWorker.add_template_created_callback.
    #
    # \~Russian
    #    \brief Удалить обработчик события создания шаблона (TemplateCreated).
    #      Потокобезопасный.
    #    \param[in] callback_id
    #      Идентификатор коллбэка, полученный от VideoWorker.add_template_created_callback.
    def remove_template_created_callback(self, callback_id):
        exception = make_exception()

        self._dll_handle.VideoWorker_removeTemplateCreatedCallback(
            self._impl,
            callback_id,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Remove a handler for MatchFound event.
    #      Thread-safe.
    #    \param[in] callback_id
    #      Integer callback id returned by VideoWorker.add_match_found_callback.
    #
    # \~Russian
    #    \brief Удалить обработчик события создания шаблона (MatchFound).
    #      Потокобезопасный.
    #    \param[in] callback_id
    #      Идентификатор коллбэка, полученный от VideoWorker.add_match_found_callback.
    def remove_match_found_callback(self, callback_id):
        exception = make_exception()

        self._dll_handle.VideoWorker_removeMatchFoundCallback(
            self._impl,
            callback_id,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Remove a handler for StiPersonOutdated event.
    #      Thread-safe.
    #    \param[in] callback_id
    #      Integer callback id returned by VideoWorker.add_sti_person_outdated_callback.
    #
    # \~Russian
    #    \brief Удалить обработчик события создания шаблона (StiPersonOutdated).
    #      Потокобезопасный.
    #    \param[in] callback_id
    #      Идентификатор коллбэка, полученный от VideoWorker.add_sti_person_outdated_callback.
    def remove_sti_person_outdated_callback(self, callback_id):
        exception = make_exception()

        self._dll_handle.VideoWorker_removeStiPersonOutdatedCallback(
            self._impl,
            callback_id,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Set / replace the database (can be called at any time).
    #      Available only if matching_threads_count > 0.
    #      Thread-safe.
    #    \param[in] elements
    #      Vector of database elements.
    #    \param[in] acceleration
    #      Acceleration type.
    #
    # \~Russian
    #    \brief Задать / заменить базу (можно вызывать в любое время).
    #      Доступно только при ненулевом количестве потоков сравнения (matching_threads_count).
    #      Потокобезопасный.
    #    \param[in] elements
    #      Вектор элементов базы.
    #    \param[in] acceleration
    #      Тип ускорения поиска.
    def set_database(self,
                     elements: List[DatabaseElement],
                     acceleration: SearchAccelerationType = SearchAccelerationType.SEARCH_ACCELERATION_1):
        element_ids_v = (c_uint64 * len(elements))()
        person_ids_v = (c_uint64 * len(elements))()
        thresholds_v = (c_float * len(elements))()
        templates_v = (c_void_p * len(elements))()

        for i, el in enumerate(elements):
            element_ids_v[i] = el.element_id
            person_ids_v[i] = el.person_id
            thresholds_v[i] = el.distance_threshold
            templates_v[i] = el.face_template._impl

        exception = make_exception()

        self._dll_handle.VideoWorker_setDatabase(
            self._impl,
            c_int32(SearchAccelerationType(acceleration).value),
            c_int32(len(elements)),
            element_ids_v,
            person_ids_v,
            templates_v,
            thresholds_v,
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Get a size N of tracking conveyor.
    #      Tracking callback will be called with frame_id = X not earlier than
    #      VideoWorker.add_video_frame returns the value X + N - 1, where N is the value
    #      returned by VideoWorker.get_tracking_conveyor_size.
    #      Thread-safe.
    #    \param[in] stream_id
    #      Integer id of the video stream
    #      (0 <= stream_id < streams_count).
    #    \return Size of tracking conveyor.
    #
    # \~Russian
    #    \brief Получить размер N конвейера трекинга.
    #      Tracking коллбэк будет вызван с frame_id = X не раньше, чем
    #      VideoWorker.add_video_frame вернет значение X + N - 1, где N - значение,
    #      которое вернул VideoWorker.get_tracking_conveyor_size.
    #      Потокобезопасный.
    #    \param[in] stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    #    \return Размер конвейера трекинга.
    def get_tracking_conveyor_size(self, stream_id: int) -> int:
        exception = make_exception()

        result = self._dll_handle.VideoWorker_getTrackingConveyorSize(
            self._impl,
            stream_id,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Get a method name of the recognizer used. Thread-safe.
    #    \return The name of the method.
    # \~Russian
    #    \brief Получить имя метода используемого распознавателя. Потокобезопасный.
    #    \return Имя метода.
    def get_method_name(self) -> str:
        name_stream_wrap = BytesIO()

        exception = make_exception()
        self._dll_handle.VideoWorker_getMethodName(
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
    #    \brief Get a number of video streams.
    #      Thread-safe.
    #    \return Number of video streams.
    #
    # \~Russian
    #    \brief Получить количество видеопотоков.
    #      Потокобезопасный.
    #    \return Количество видеопотоков.
    def get_streams_count(self):
        exception = make_exception()

        result = self._dll_handle.VideoWorker_getStreamsCount(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    @staticmethod
    @CFUNCTYPE(None, c_void_p, py_object, c_void_p, c_int32, c_void_p, c_void_p, c_int32, c_void_p, c_void_p)
    def s_tracking_callback_u(err_stream: int,
                              self,
                              callback_data: int,
                              callbacks_count: int,
                              callbacks_func: int,
                              callbacks_userdata: int,
                              u_callbacks_count: int,
                              u_callbacks_func: int,
                              u_callbacks_userdata: int):
        try:
            if self is None:
                raise Error(0x22bec9af, 'zero self in s_tracking_callback_u')

            exception = make_exception()

            stream_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_stream_id_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_frame_id_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_count = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_count_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_impls = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            weak_samples = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_weak_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_quality = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_quality_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_good_light_and_blur = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_good_light_and_blur_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_good_angles = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_good_angles_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_good_face_size = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_good_face_size_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_detector_confirmed = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_detector_confirmed_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_depth_liveness_confirmed = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_depth_liveness_confirmed_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_ir_liveness_confirmed = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_ir_liveness_confirmed_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_age_gender_set = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_age_gender_set_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_gender = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_gender_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_age = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_age_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_age_years = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_age_years_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_emotions_set = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_emotions_set_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_emotions_count = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_emotions_count_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_emotions_confidence = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_emotions_confidence_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_emotions_emotion = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_track_emotions_emotion_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_active_liveness_type = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_active_liveness_type_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_active_liveness_confirmed = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_active_liveness_confirmed_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            samples_track_active_liveness_progress = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_active_liveness_score_samples_t.value,
                exception)
            check_exception(exception, self._dll_handle)

            data = TrackingCallbackData()
            data.stream_id = stream_id
            data.frame_id = frame_id
            data.samples_track_age_gender = [0] * samples_count
            data.samples_track_emotions = [[]] * samples_count
            data.samples_active_liveness_status = [None] * samples_count

            p_size = sizeof(c_void_p)
            c_int32_size = sizeof(c_int32)
            c_float_size = sizeof(c_float)

            emotions_i = 0
            for i in range(samples_count):
                sample = RawSample(self._dll_handle, c_void_p.from_address(samples_impls + i * p_size))
                data.samples.append(sample)
                data.samples_track_id.append(sample.get_id())

                weak_sample = c_int32.from_address(weak_samples + i * c_int32_size).value != 0
                data.samples_weak.append(weak_sample)

                sample_quality = c_float.from_address(samples_quality + i * c_float_size).value
                data.samples_quality.append(sample_quality)

                sample_good_light_and_blur = Verdict(
                    c_int32.from_address(samples_good_light_and_blur + i * c_int32_size).value)
                data.samples_good_light_and_blur.append(sample_good_light_and_blur)

                sample_good_angles = Verdict(c_int32.from_address(samples_good_angles + i * c_int32_size).value)
                data.samples_good_angles.append(sample_good_angles)

                sample_good_face_size = Verdict(c_int32.from_address(samples_good_face_size + i * c_int32_size).value)
                data.samples_good_face_size.append(sample_good_face_size)

                sample_detector_confirmed = Verdict(
                    c_int32.from_address(samples_detector_confirmed + i * c_int32_size).value)
                data.samples_detector_confirmed.append(sample_detector_confirmed)

                sample_depth_liveness_confirmed = depth_liveness_estimator.Liveness(
                    c_int32.from_address(samples_depth_liveness_confirmed + i * c_int32_size).value)
                data.samples_depth_liveness_confirmed.append(sample_depth_liveness_confirmed)

                sample_ir_liveness_confirmed = ir_liveness_estimator.Liveness(
                    c_int32.from_address(samples_ir_liveness_confirmed + i * c_int32_size).value)
                data.samples_ir_liveness_confirmed.append(sample_ir_liveness_confirmed)
                samples_active_liveness_status = active_liveness.ActiveLivenessStatus(
                    active_liveness.Liveness(
                        c_int32.from_address(samples_track_active_liveness_confirmed + i * c_int32_size).value),
                    active_liveness.CheckType(
                        c_int32.from_address(samples_track_active_liveness_type + i * c_int32_size).value),
                    c_float.from_address(samples_track_active_liveness_progress + i * c_float_size).value)
                data.samples_active_liveness_status[i] = samples_active_liveness_status

                sample_track_age_gender_set = c_int32.from_address(
                    samples_track_age_gender_set + i * c_int32_size).value != 0
                data.samples_track_age_gender_set.append(sample_track_age_gender_set)

                if sample_track_age_gender_set:
                    gender = Gender(c_int32.from_address(samples_track_gender + i * c_int32_size).value)
                    age = Age(c_int32.from_address(samples_track_age + i * c_int32_size).value)
                    age_years = c_float.from_address(samples_track_age_years + i * c_float_size).value

                    gender = Gender(gender)
                    age = Age(age)
                    age_gender = AgeGender(age, gender, age_years)

                    data.samples_track_age_gender[i] = age_gender

                sample_track_emotions_set = c_int32.from_address(
                    samples_track_emotions_set + i * c_int32_size).value != 0
                data.samples_track_emotions_set.append(sample_track_emotions_set)

                if sample_track_emotions_set:
                    emotions_count = c_int32.from_address(samples_track_emotions_count + i * c_int32_size).value
                    for k in range(emotions_count):
                        emotion = c_int32.from_address(samples_track_emotions_emotion + emotions_i * c_int32_size).value
                        confidence = c_float.from_address(
                            samples_track_emotions_confidence + emotions_i * c_float_size).value

                        data.samples_track_emotions[i].append(EmotionConfidence(emotion, confidence))

                        emotions_i += 1

            for i in range(u_callbacks_count):
                try:
                    u_callback_function_i = py_object.from_address(u_callbacks_func + i * p_size).value
                    u_callback_function_i(data, py_object.from_address(u_callbacks_userdata + i * p_size).value)
                except Exception as ex:
                    error_str = 'VideoWorker TrackingCallbackU catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker TrackingCallbackU catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

            for i in range(callbacks_count):
                try:
                    callback_function_i = py_object.from_address(callbacks_func + i * p_size).value
                    callback_function_i(data.stream_id,
                                        data.frame_id,
                                        data.samples,
                                        data.samples_weak,
                                        data.samples_quality,
                                        py_object.from_address(callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker TrackingCallback catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker TrackingCallback catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

        except Exception as ex:
            error_str = 'VideoWorker TrackingCallback_ catch Exception: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

        except BaseException as ex:
            error_str = 'VideoWorker TrackingCallback_ catch BaseException: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

    @staticmethod
    @CFUNCTYPE(None, c_void_p, py_object, c_void_p, c_int32, c_void_p, c_void_p, c_int32, c_void_p, c_void_p)
    def s_template_created_callback_u(err_stream: int,
                                      self,
                                      callback_data: int,
                                      callbacks_count: int,
                                      callbacks_func: int,
                                      callbacks_userdata: int,
                                      u_callbacks_count: int,
                                      u_callbacks_func: int,
                                      u_callbacks_userdata: int):
        try:
            if self is None:
                raise Error(0x4e9a2681, 'zero self in s_template_created_callback_u')

            exception = make_exception()

            stream_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_stream_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_frame_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            quality = self._dll_handle.StructStorage_get_double(
                c_void_p(callback_data),
                StructStorageFields.video_worker_quality_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sample_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            templ_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_templ_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            p_size = sizeof(c_void_p)

            sample = RawSample(self._dll_handle, c_void_p(sample_impl))
            templ = Template(self._dll_handle, c_void_p(templ_impl))
            data = TemplateCreatedCallbackData(stream_id, frame_id, quality, sample, templ)

            for i in range(u_callbacks_count):
                try:
                    u_callback_function_i = py_object.from_address(u_callbacks_func + i * p_size).value
                    u_callback_function_i(data, py_object.from_address(u_callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker TemplateCreatedCallbackU catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))
                    raise Exception(error_str)

                except BaseException as ex:
                    error_str = 'VideoWorker TemplateCreatedCallbackU catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))
                    raise Exception(error_str)

            for i in range(callbacks_count):
                try:
                    callback_function_i = py_object.from_address(callbacks_func + i * p_size).value
                    callback_function_i(data.stream_id,
                                        data.frame_id,
                                        data.sample,
                                        data.quality,
                                        data.templ,
                                        py_object.from_address(callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker TemplateCreatedCallback catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))
                    raise Exception(error_str)

                except BaseException as ex:
                    error_str = 'VideoWorker TemplateCreatedCallback catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))
                    raise Exception(error_str)

        except Exception as ex:
            error_str = 'VideoWorker TemplateCreatedCallback_ catch Exception: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))
            raise Exception(error_str)

        except BaseException as ex:
            error_str = 'VideoWorker TemplateCreatedCallback_ catch BaseException: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))
            raise Exception(error_str)

    @staticmethod
    @CFUNCTYPE(None, c_void_p, py_object, c_void_p, c_int32, c_void_p, c_void_p, c_int32, c_void_p, c_void_p,
               c_int32, c_void_p, c_void_p)
    def s_match_found_callback_u(err_stream: int,
                                 self,
                                 callback_data: int,
                                 callbacks_count: int,
                                 callbacks_func: int,
                                 callbacks_userdata: int,
                                 ext_callbacks_count: int,
                                 ext_callbacks_func: int,
                                 ext_callbacks_userdata: int,
                                 u_callbacks_count: int,
                                 u_callbacks_func: int,
                                 u_callbacks_userdata: int):
        try:
            if self is None:
                raise Error(0x3c512755, 'zero self in s_match_found_callback_u')

            exception = make_exception()

            stream_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_stream_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_frame_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            quality = self._dll_handle.StructStorage_get_double(
                c_void_p(callback_data),
                StructStorageFields.video_worker_quality_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sample_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            templ_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_templ_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_size = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_size_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_person_ids = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_person_ids_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_element_ids = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_element_ids_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_match_result_distances = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_match_result_distance_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_match_result_far_s = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_match_result_fa_r_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_match_result_frr_s = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_match_result_fr_r_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            search_result_match_result_scores = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_search_result_match_result_score_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sample = RawSample(self._dll_handle, c_void_p(sample_impl))
            templ = Template(self._dll_handle, c_void_p(templ_impl))

            p_size = sizeof(c_void_p)
            uint64_size = sizeof(c_uint64)
            int64_size = sizeof(c_int64)
            float_size = sizeof(c_float)

            search_result = list()
            for i in range(search_result_size):
                search_result.append(
                    SearchResult(c_int64.from_address(search_result_person_ids + i * int64_size).value,
                                 c_int64.from_address(search_result_element_ids + i * int64_size).value,
                                 MatchResult(
                                     c_float.from_address(search_result_match_result_distances + i * float_size).value,
                                     c_float.from_address(search_result_match_result_far_s + i * float_size).value,
                                     c_float.from_address(search_result_match_result_frr_s + i * float_size).value,
                                     c_float.from_address(search_result_match_result_scores + i * float_size).value)))

            data = MatchFoundCallbackData(stream_id, frame_id, quality, sample, templ, search_result)

            for i in range(u_callbacks_count):
                try:
                    u_callback_function_i = py_object.from_address(u_callbacks_func + i * p_size).value
                    u_callback_function_i(data, py_object.from_address(u_callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker MatchFoundCallbackU catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker MatchFoundCallbackU catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

            for i in range(ext_callbacks_count):
                try:
                    ext_callback_function_i = py_object.from_address(ext_callbacks_func + i * p_size).value
                    ext_callback_function_i(data.stream_id,
                                            data.frame_id,
                                            data.sample,
                                            data.quality,
                                            data.templ,
                                            data.search_results,
                                            py_object.from_address(ext_callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker MatchFoundCallbackExt catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker MatchFoundCallbackExt catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

            for i in range(callbacks_count):
                try:
                    callback_function_i = py_object.from_address(callbacks_func + i * p_size).value
                    callback_function_i(data.stream_id,
                                        data.frame_id,
                                        data.search_results[0].person_id,
                                        data.search_results[0].element_id,
                                        data.sample,
                                        data.quality,
                                        data.templ,
                                        data.search_results[0].match_result,
                                        py_object.from_address(callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker MatchFoundCallback catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker MatchFoundCallback catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

        except Exception as ex:
            error_str = 'VideoWorker MatchFoundCallback_ catch Exception: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

        except BaseException as ex:
            error_str = 'VideoWorker MatchFoundCallback_ catch BaseException: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

    @staticmethod
    @CFUNCTYPE(None, c_void_p, py_object, c_void_p, c_int32, c_void_p, c_void_p)
    def s_sti_person_outdated_callback_u(err_stream: c_void_p,
                                         self: py_object,
                                         callback_data: c_void_p,
                                         callbacks_count: c_int32,
                                         callbacks_func: c_void_p,
                                         callbacks_userdata: c_void_p):
        try:
            if self is None:
                raise Error(0xf8773704, 'zero self in s_sti_person_outdated_callback_u')

            exception = make_exception()

            stream_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_stream_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sti_person_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_sti_person_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            data = StiPersonOutdatedCallbackData(stream_id, sti_person_id)

            p_size = sizeof(c_void_p)
            for i in range(callbacks_count):
                try:
                    callback_function_i = py_object.from_address(callbacks_func + i * p_size).value
                    callback_function_i(data, py_object.from_address(callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker StiPersonOutdated catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker StiPersonOutdated catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

        except Exception as ex:
            error_str = 'VideoWorker StiPersonOutdated catch Exception: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

        except BaseException as ex:
            error_str = 'VideoWorker StiPersonOutdated catch BaseException: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

    @staticmethod
    @CFUNCTYPE(None, c_void_p, py_object, c_void_p, c_int32, c_void_p, c_void_p, c_int32, c_void_p, c_void_p)
    def s_tracking_lost_callback_u(err_stream: int,
                                   self,
                                   callback_data: int,
                                   callbacks_count: int,
                                   callbacks_func: int,
                                   callbacks_userdata: int,
                                   u_callbacks_count: int,
                                   u_callbacks_func: int,
                                   u_callbacks_userdata: int):
        try:
            if self is None:
                raise Error(0xd74f2df7, 'zero self in s_tracking_lost_callback_u')

            exception = make_exception()

            stream_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_stream_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            first_frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_first_frame_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            last_frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_last_frame_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            best_quality = self._dll_handle.StructStorage_get_double(
                c_void_p(callback_data),
                StructStorageFields.video_worker_best_quality_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            best_quality_frame_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_best_qaulity_frame_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            best_quality_sample_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_samples_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            best_quality_template_impl = self._dll_handle.StructStorage_get_pointer(
                c_void_p(callback_data),
                StructStorageFields.video_worker_templ_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            track_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_track_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sti_person_id_set = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_sti_person_id_set_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sti_person_id = self._dll_handle.StructStorage_get_int64(
                c_void_p(callback_data),
                StructStorageFields.video_worker_sti_person_id_t.value,
                exception)

            check_exception(exception, self._dll_handle)

            sample = RawSample(self._dll_handle, c_void_p(best_quality_sample_impl))
            templ = Template(self._dll_handle, c_void_p(best_quality_template_impl))

            data = TrackingLostCallbackData()
            data.stream_id = stream_id
            data.first_frame_id = first_frame_id
            data.last_frame_id = last_frame_id
            data.best_quality = best_quality
            data.best_quality_frame_id = best_quality_frame_id
            data.best_quality_sample = sample
            data.best_quality_templ = templ
            data.track_id = track_id
            data.sti_person_id_set = (sti_person_id_set != 0)
            data.sti_person_id = sti_person_id

            p_size = sizeof(c_void_p)
            for i in range(u_callbacks_count):
                try:
                    u_callback_function_i = py_object.from_address(u_callbacks_func + i * p_size).value
                    u_callback_function_i(data, py_object.from_address(u_callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker TrackingLostCallbackU catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker TrackingLostCallbackU catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

            for i in range(callbacks_count):
                try:
                    callback_function_i = py_object.from_address(callbacks_func + i * p_size).value
                    callback_function_i(data.stream_id,
                                        data.first_frame_id,
                                        data.last_frame_id,
                                        data.best_quality,
                                        data.best_quality_frame_id,
                                        data.best_quality_sample,
                                        data.best_quality_templ,
                                        py_object.from_address(callbacks_userdata + i * p_size).value)

                except Exception as ex:
                    error_str = 'VideoWorker TrackingLostCallback catch Exception: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

                except BaseException as ex:
                    error_str = 'VideoWorker TrackingLostCallback catch BaseException: "{}"'.format(ex)
                    self._dll_handle.VideoWorker_errStreamWriteFunc(
                        c_void_p(err_stream),
                        POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                        c_uint64(len(error_str)))

        except Exception as ex:
            error_str = 'VideoWorker TrackingLostCallback_ catch Exception: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

        except BaseException as ex:
            error_str = 'VideoWorker TrackingLostCallback_ catch BaseException: "{}"'.format(ex)
            self._dll_handle.VideoWorker_errStreamWriteFunc(
                c_void_p(err_stream),
                POINTER(c_char_p)(create_string_buffer(error_str.encode())),
                c_uint64(len(error_str)))

    def toggle_something(self, stream_id: int, something: VideoWorkerSomething):
        exception = make_exception()

        self._dll_handle.VideoWorker_toggleSomething(
            self._impl,
            c_int32(stream_id),
            c_int32(something),
            exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Disable the creation of templates for a given stream.
    #      After that, the matching stops as well.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Отключить генерацию шаблонов для данного потока.
    #      Соответственно, сравнения также остановятся.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def disable_processing_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.Processing_Disable)

    ##
    # \~English
    #    \brief Enable the creation of templates for a given stream.
    #      After that, the matching is resumed as well.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Включить генерацию шаблонов для данного потока.
    #      Соответственно, сравнения также возобновятся.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def enable_processing_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.Processing_Enable)

    ##
    # \~English
    #    \brief Disable age and gender estimation for a given stream.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Отключить определение пола и возраста для данного потока.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def disable_age_gender_estimation_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.AgeGender_Disable)

    ##
    # \~English
    #    \brief Enable age and gender estimation for a given stream.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Включить определение пола и возраста для данного потока.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def enable_age_gender_estimation_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.AgeGender_Enable)

    ##
    # \~English
    #    \brief Disable estimation of emotions for a given stream.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Отключить определение эмоций для данного потока.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def disable_emotions_estimation_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.Emotions_Disable)

    ##
    # \~English
    #    \brief Enable estimation of emotions for a given stream.
    #      Thread-safe.
    #
    #   \param[in]  stream_id
    #     Integer id of the video stream
    #     (0 <= stream_id < streams_count).
    #
    # \~Russian
    #    \brief Включить определение эмоций для данного потока.
    #      Потокобезопасный.
    #
    #    \param[in]  stream_id
    #      Целочисленный идентификатор видеопотока
    #      (0 <= stream_id < streams_count).
    def enable_emotions_estimation_on_stream(self, stream_id: int):
        self.toggle_something(stream_id, VideoWorkerSomething.Emotions_Enable)

    ##
    # \~English
    #    \brief Change the parameter value in runtime.
    #
    #    \param[in] parameter
    #      Parameter name (tag name in the .xml config file).
    #
    #    \param[in] value
    #      New parameter value.
    #
    # \~Russian
    #    \brief Изменить значение параметра в runtime.
    #
    #    \param[in] parameter
    #      Имя параметра (имя тэга из .xml конфигурационного файла).
    #
    #    \param[in] value
    #      Новое значение параметра.
    def set_parameter(self, parameter: str, value: float):
        exception = make_exception()

        self._dll_handle.VideoWorker_toggleSomething(
            self._impl,
            c_char_p(bytes(parameter, "ascii")),
            c_double(value),
            exception)

        check_exception(exception, self._dll_handle)