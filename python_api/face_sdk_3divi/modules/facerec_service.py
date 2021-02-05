##
#  \file facerec_service.py
#  \~English
#     \brief FacerecService - Interface object for creating other interface objects.
#  \~Russian
#     \brief FacerecService - Интерфейсный объект для создания других интерфейсных объектов.

from ctypes import CDLL
from ctypes import c_char_p, c_void_p, c_int32, c_uint32, c_float, POINTER
from ctypes import create_string_buffer
from ctypes import c_int, c_double
from ctypes import py_object

from io import BytesIO
from typing import Union

from .exception_check import check_exception, make_exception
from .age_gender_estimator import AgeGenderEstimator
from .emotions_estimator import EmotionsEstimator
from .face_quality_estimator import FaceQualityEstimator
from .quality_estimator import QualityEstimator
from .depth_liveness_estimator import DepthLivenessEstimator
from .ir_liveness_estimator import IRLivenessEstimator
from .liveness_2d_estimator import Liveness2DEstimator
from .wrap_funcs import read_func
from .complex_object import ComplexObject
from .recognizer import Recognizer
from .dll_handle import DllHandle
from .raw_sample import RawSample
from .capturer import Capturer
from .config import Config
from .video_worker import VideoWorker, Params
from .error import Error
from .wrap_funcs import write_func


##
# \~English
#    \brief Interface object for creating other interface objects.
# \~Russian
#    \brief Интерфейсный объект для создания других интерфейсных объектов.
class FacerecService(ComplexObject):

    def __init__(self, dll_handle: CDLL, facerec_conf_dir: str, impl: c_void_p):
        super(FacerecService, self).__init__(dll_handle, impl)
        self.__facerec_conf_dir = facerec_conf_dir

    ##
    #  \~English
    #     \brief Initializes the facerec lib (can be called only once).
    #
    #     \param[in] dll_path
    #       An absolute or a relative path to libfacerec.so on Linux or facerec.dll on Windows.
    #
    #     \param[in] facerec_conf_dir
    #       An absolute or a relative path to the directory with config files (the 'face_sdk/conf/facerec/' directory).
    #
    #     \param[in] license_dir
    #       If you need to specify the directory containing the license file, then it's an absolute
    #       or a relative path to this directory, otherwise, it's an empty string.
    #       In case of an empty string, a license file is searched first in the 'face_sdk/license/' directory. If it's not found, it's searched in the 'face_sdk/conf/facerec/' directory.
    #
    #     \return Created FacerecService object.
    #
    #  \~Russian
    #     \brief Инициализировать работу с библиотекой libfacerec (нельзя вызывать более одного раза).
    #
    #     \param[in] dll_path
    #       Абсолютный или относительный путь до файла билиотеки - libfacerec.so на Linux или facerec.dll на Windows.
    #
    #     \param[in] facerec_conf_dir
    #       Абсолютный или относительный путь до каталога с конфигурационными файлами (каталог 'face_sdk/conf/facerec/' в дистрибутиве).
    #
    #     \param[in] license_dir
    #       Если требуется указать директорию с файлом лицензии, то абсолютный
    #       или относительный путь до этой директории, иначе - пустая строка.
    #       В случае пустой строки файл лицензии ищется сначала в каталоге 'face_sdk/license/', затем в каталоге 'face_sdk/conf/facerec/'.
    #
    #     \return Созданный объект FacerecService.
    @classmethod
    def create_service(cls, dll_path: str, facerec_conf_dir: str, online_license_dir: str = ''):
        if facerec_conf_dir == '':
            facerec_conf_dir = './'
        elif facerec_conf_dir[len(facerec_conf_dir) - 1] != '/':
            facerec_conf_dir += '/'

        dll_handle = DllHandle(CDLL(dll_path))

        exception = make_exception()

        the_impl = dll_handle.FacerecService_constructor2(
            POINTER(c_char_p)(create_string_buffer(facerec_conf_dir.encode())),
            None if len(online_license_dir) == 0 else POINTER(c_char_p)(
                create_string_buffer(online_license_dir.encode())),
            POINTER(c_char_p)(create_string_buffer(dll_path.encode())),
            exception)

        check_exception(exception, dll_handle)
        return cls(dll_handle, facerec_conf_dir, c_void_p(the_impl))

    ##
    # \~English
    #    \brief Get version of face recognition library.
    #      Thread-safe.
    #
    #    \return The name of the method.
    #
    # \~Russian
    #    \brief Получить имя метода.
    #      Потокобезопасный.
    #
    #    \return Имя метода.
    def get_version(self) -> str:
        version_stream = BytesIO()

        exception = make_exception()

        self._dll_handle.get_version(
            py_object(version_stream),
            write_func,
            exception)

        check_exception(exception, self._dll_handle)

        version = version_stream.getvalue()
        version_stream.close()

        return version.decode()

    ##
    #  \~English
    #     \brief Creates a Capturer object.
    #       Type and features depend on the content of the configuration file.
    #       Thread-safe.
    #     \param[in] config
    #       Сonfiguration file with optionally overridden parameters.
    #     \return Created Capturer object.
    #
    #  \~Russian
    #     \brief Создать объект Capturer.
    #       Тип и возможности зависят от указанного конфигурационного файла.
    #       Потокобезопасный.
    #     \param[in] config
    #       Конфигурационный файл с опционально переопределенными параметрами.
    #     \return Созданный объект Capturer.
    def create_capturer(self, config: Config) -> Capturer:
        assert isinstance(config, Config)
        file_path = self.__facerec_conf_dir + config.config_filepath

        overridden_keys, overridden_values = config.prepare()

        exception = make_exception()

        overridden_keys_buf = (c_char_p * len(overridden_keys))()
        overridden_values_buf = (c_double * len(overridden_values))(*overridden_values)

        for i in range(len(overridden_keys)):
            overridden_keys_buf[i] = c_char_p(overridden_keys[i].encode())

        capturer_impl = self._dll_handle.FacerecService_createCapturerE(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            len(overridden_keys),
            overridden_keys_buf if len(overridden_keys) else None,
            overridden_values_buf if len(overridden_values) else None,
            exception)

        check_exception(exception, self._dll_handle)

        return Capturer(self._dll_handle, c_void_p(capturer_impl))

    ##
    # \~English
    #    \brief Creates a AgeGenderEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the config file.
    #
    #    \return Created AgeGenderEstimator object.
    #
    # \~Russian
    #    \brief Создать объект AgeGenderEstimator.
    #      Потокобезопасный.
    #
    #    \param[in]  ini_file
    #      Имя конфигурационного файла.
    #
    #    \return Созданный объект AgeGenderEstimators.
    def create_age_gender_estimator(self, ini_file: str) -> AgeGenderEstimator:
        file_path = self.__facerec_conf_dir + ini_file

        exception = make_exception()

        agegender_impl = self._dll_handle.FacerecService_createAgeGenderEstimator(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            exception)

        check_exception(exception, self._dll_handle)

        return AgeGenderEstimator(self._dll_handle, c_void_p(agegender_impl))

    ##
    # \~English
    #    \brief Creates a EmotionsEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the config file.
    #
    #    \return Created EmotionsEstimator object.
    #
    # \~Russian
    #    \brief Создать объект EmotionsEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла.
    #
    #    \return Созданный объект EmotionsEstimator.
    def create_emotions_estimator(self, ini_file: str) -> EmotionsEstimator:
        file_path = self.__facerec_conf_dir + ini_file

        exception = make_exception()

        emotions_impl = self._dll_handle.FacerecService_createEmotionsEstimator(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            exception)

        check_exception(exception, self._dll_handle)

        return EmotionsEstimator(self._dll_handle, c_void_p(emotions_impl))

    ##
    # \~English
    #    \brief Creates a FaceQualityEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the config file.
    #
    #    \return Created FaceQualityEstimator object.
    #
    # \~Russian
    #    \brief Создать объект FaceQualityEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла.
    #
    #    \return Созданный объект FaceQualityEstimator.
    def create_face_quality_estimator(self, ini_file: str) -> FaceQualityEstimator:
        file_path = self.__facerec_conf_dir + ini_file

        exception = make_exception()

        facequality_impl = self._dll_handle.FacerecService_createFaceQualityEstimator(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            exception)

        check_exception(exception, self._dll_handle)

        return FaceQualityEstimator(self._dll_handle, c_void_p(facequality_impl))

    ##
    # \~English
    #    \brief Creates a  QualityEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the config file.
    #
    #    \return Created QualityEstimator object.
    #
    # \~Russian
    #    \brief Создать объект QualityEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла.
    #
    #    \return Созданный объект QualityEstimator.
    def create_quality_estimator(self, ini_file: str) -> QualityEstimator:
        file_path = self.__facerec_conf_dir + ini_file

        exception = make_exception()

        quality_impl = self._dll_handle.FacerecService_createQualityEstimator(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            exception)

        check_exception(exception, self._dll_handle)

        return QualityEstimator(self._dll_handle, c_void_p(quality_impl))

    ##
    # \~English
    #    \brief Creates a DepthLivenessEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the configuration file or configuration file with optionally overridden parameters.
    #
    #    \return Created DepthLivenessEstimator object.
    #
    # \~Russian
    #    \brief Создать объект DepthLivenessEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла или конфигурационный файл с опционально переопределенными параметрами
    #
    #    \return Созданный объект DepthLivenessEstimator.
    def create_depth_liveness_estimator(self, config: Union[str, Config]) -> DepthLivenessEstimator:
        if isinstance(config, Config):
            file_path = self.__facerec_conf_dir + config.config_filepath
            overridden_keys, overridden_values = config.prepare()
        else:
            file_path = self.__facerec_conf_dir + config
            overridden_keys, overridden_values = [], []

        exception = make_exception()

        overridden_keys_buf = (c_char_p * len(overridden_keys))()
        overridden_values_buf = (c_double * len(overridden_values))(*overridden_values)

        for i in range(len(overridden_keys)):
            overridden_keys_buf[i] = c_char_p(overridden_keys[i].encode())

        depth_liveness_impl = self._dll_handle.FacerecService_createDepthLivenessEstimatorE(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            c_int32(len(overridden_keys)),
            overridden_keys_buf if len(overridden_keys) else None,
            overridden_values_buf if len(overridden_values) else None,
            exception)

        check_exception(exception, self._dll_handle)

        return DepthLivenessEstimator(self._dll_handle, c_void_p(depth_liveness_impl))

    ##
    # \~English
    #    \brief Creates an IRLivenessEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the configuration file or configuration file with optionally overridden parameters.
    #
    #    \return Created IRLivenessEstimator object.
    #
    # \~Russian
    #    \brief Создать объект IRLivenessEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла или конфигурационный файл с опционально переопределенными параметрами
    #
    #    \return Созданный объект IRLivenessEstimator.
    def create_ir_liveness_estimator(self, config: Union[str, Config]) -> IRLivenessEstimator:
        if isinstance(config, Config):
            file_path = self.__facerec_conf_dir + config.config_filepath
            overridden_keys, overridden_values = config.prepare()
        else:
            file_path = self.__facerec_conf_dir + config
            overridden_keys, overridden_values = [], []

        exception = make_exception()

        overridden_keys_buf = (c_char_p * len(overridden_keys))()
        overridden_values_buf = (c_double * len(overridden_values))(*overridden_values)

        for i in range(len(overridden_keys)):
            overridden_keys_buf[i] = c_char_p(overridden_keys[i].encode())

        ir_liveness_impl = self._dll_handle.FacerecService_createIRLivenessEstimatorE(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            c_int32(len(overridden_keys)),
            overridden_keys_buf if len(overridden_keys) else None,
            overridden_values_buf if len(overridden_values) else None,
            exception)

        check_exception(exception, self._dll_handle)

        return IRLivenessEstimator(self._dll_handle, c_void_p(ir_liveness_impl))

    ##
    # \~English
    #    \brief Liveness2DEstimator object.
    #      Thread-safe.
    #
    #    \param[in] ini_file
    #      Name of the configuration file or configuration file with optionally overridden parameters.
    #
    #    \return Created Liveness2DEstimator object.
    #
    # \~Russian
    #    \brief Создать объект Liveness2DEstimator.
    #      Потокобезопасный.
    #
    #    \param[in] ini_file
    #      Имя конфигурационного файла или конфигурационный файл с опционально переопределенными параметрами
    #
    #    \return Созданный объект Liveness2DEstimator.
    def create_liveness_2d_estimator(self, config: Union[str, Config]) -> Liveness2DEstimator:
        if isinstance(config, Config):
            file_path = self.__facerec_conf_dir + config.config_filepath
            overridden_keys, overridden_values = config.prepare()
        else:
            file_path = self.__facerec_conf_dir + config
            overridden_keys, overridden_values = [], []

        exception = make_exception()

        overridden_keys_buf = (c_char_p * len(overridden_keys))()
        overridden_values_buf = (c_double * len(overridden_values))(*overridden_values)

        for i in range(len(overridden_keys)):
            overridden_keys_buf[i] = c_char_p(overridden_keys[i].encode())

        liveness_2d_impl = self._dll_handle.FacerecService_createLiveness2DEstimatorE(
            self._impl,
            POINTER(c_char_p)(create_string_buffer(file_path.encode())),
            c_int32(len(overridden_keys)),
            overridden_keys_buf if len(overridden_keys) else None,
            overridden_values_buf if len(overridden_values) else None,
            exception)

        check_exception(exception, self._dll_handle)

        return Liveness2DEstimator(self._dll_handle, c_void_p(liveness_2d_impl))

    ##
    # \~English
    #    \brief Creates a Recognizer object.
    #      Thread-safe.
    #
    #    \param[in] recognizer_config
    #      Name of the config file or recognizer configuration file with optionally overridden parameters.
    #
    #    \param[in] processing
    #      Flag to toggle the Recognizer.processing method in the created recognizer.
    #
    #    \param[in] matching
    #      Flag to toggle the Recognizer.verify_match method in the created recognizer.
    #
    #    \param[in] processing_less_memory_consumption
    #      Flag to toggle the Recognizer.processing member
    #      function optimizations that consume a lot of RAM when creating the recognizer (see the docs).
    #
    #    \return Created Recognizer object.
    #
    # \~Russian
    #    \brief Создать объект Recognizer.
    #      Потокобезопасный.
    #
    #    \param[in] recognizer_config
    #      Имя конфигурационного файла или конфигурационный файл Recognizer с опционально переопределенными параметрами.
    #
    #    \param[in] processing
    #      Флаг для включения / выключения метода Recognizer.processing в создаваемом разпознавателе.
    #
    #    \param[in] matching
    #      Флаг для включения / выключения метода Recognizer.verify_match в создаваемом разпознавателе.
    #
    #    \param[in] processing_less_memory_consumption
    #      Флаг для выключения оптимизаций метода Recognizer.processing,
    #      потребляющих много оперативной памяти при создании распознавателя (см. документацию).
    #
    #    \return Созданный объект Recognizer.
    def create_recognizer(self, recognizer_config: Union[str, Config], matching: bool = True, processing: bool = True,
                          processing_less_memory_consumption: bool = False) -> Recognizer:
        rec_overridden_keys = []
        rec_overridden_values = []

        if isinstance(recognizer_config, str):
            file_path = recognizer_config
        else:
            file_path = recognizer_config.config_filepath
            rec_overridden_keys, rec_overridden_values = recognizer_config.prepare()

        exception = make_exception()

        rec_overridden_keys_buf = (c_char_p * len(rec_overridden_keys))()
        rec_overridden_values_buf = (c_double * len(rec_overridden_values))(*rec_overridden_values)

        for i in range(len(rec_overridden_keys)):
            rec_overridden_keys_buf[i] = c_char_p(rec_overridden_keys[i].encode())

        recognizer_impl = self._dll_handle.FacerecService_createRecognizer2(
            self._impl,
            POINTER(c_char_p)(create_string_buffer((self.__facerec_conf_dir + file_path).encode())),
            c_int32(len(rec_overridden_keys)),
            rec_overridden_keys_buf if len(rec_overridden_keys_buf) else None,
            rec_overridden_values_buf if len(rec_overridden_values_buf) else None,
            c_int(processing),
            c_int(matching),
            c_int(processing_less_memory_consumption),
            exception)

        check_exception(exception, self._dll_handle)

        return Recognizer(self._dll_handle, c_void_p(recognizer_impl))

    ##
    # \~English
    #    \brief Creates a VideoWorker object.
    #      Thread-safe.<br>
    #      When VideoWorker is created with <i>matching_thread=0</i> and <i>processing_thread=0</i>,
    #      then the standard Capturer license is used. <br>Depending on the settings, VideoWorker uses either the
    #      <i>VideoClient</i> license (face tracking on video streams) or the <i>VideoClientExt</i> license (face tracking, template
    #      creation and matching with the database) (see <a href="https://github.com/3DiVi/face-sdk-docs/blob/master/doc/en/components.md">Components</a> for details).
    #
    #    \param[in] params
    #      Parameters of the VideoWorker constructor.
    #
    #    \return Created VideoWorker object.
    #
    # \~Russian
    #    \brief Создать объект VideoWorker.
    #      Потокобезопасный.<br>
    #      Если при создании VideoWorker указаны параметры <i>matching_thread=0</i> и
    #      <i>processing_thread=0</i>, то потребляется обычная лицензия Capturer. <br>В зависимости от настроек,
    #      VideoWorker потребляет лицензию <i>VideoClient</i> (детекция лиц на видеопотоках)
    #      либо <i>VideoClientExt</i> (детекция лиц на видеопотоках, создание шаблонов и сравнение с базой) (см. <a href="https://github.com/3DiVi/face-sdk-docs/blob/master/doc/ru/#components.md">Компоненты</a>).
    #
    #    \param[in] params
    #      Параметры конструктора VideoWorker.
    #
    #    \return Созданный объект VideoWorker.
    def create_video_worker(self, params: Params) -> VideoWorker:
        assert params.video_worker_config is not None
        video_worker_file_path = self.__facerec_conf_dir + params.video_worker_config.config_filepath

        vw_overridden_keys, vw_overridden_values = params.video_worker_config.prepare()

        exception = make_exception()

        vw_overridden_keys_buf = (c_char_p * len(vw_overridden_keys))()
        vw_overridden_values_buf = (c_double * len(vw_overridden_values))(*vw_overridden_values)

        for i in range(len(vw_overridden_keys)):
            vw_overridden_keys_buf[i] = c_char_p(vw_overridden_keys[i].encode())

        if params.recognizer_ini_file and \
            params.recognizer_config.config_filepath:
            raise Error(0xed877a99, "You must use either recognizer_config or recognizer_ini_file")

        recognizer_config = Config(params.recognizer_ini_file) if params.recognizer_ini_file != "" else params.recognizer_config
        rec_overridden_keys, rec_overridden_values = recognizer_config.prepare()

        rec_overridden_keys_buf = (c_char_p * len(rec_overridden_keys))()
        rec_overridden_values_buf = (c_double * len(rec_overridden_values))(*rec_overridden_values)

        for i in range(len(rec_overridden_keys)):
            rec_overridden_keys_buf[i] = c_char_p(rec_overridden_keys[i].encode())

        vw_impl = self._dll_handle.FacerecService_createVideoWorkerStiAgeGenderEmotions(
            self._impl,

            VideoWorker.s_tracking_callback_u,
            VideoWorker.s_template_created_callback_u,
            VideoWorker.s_match_found_callback_u,
            VideoWorker.s_tracking_lost_callback_u,
            VideoWorker.s_sti_person_outdated_callback_u,

            POINTER(c_char_p)(create_string_buffer(video_worker_file_path.encode())),
            c_int32(len(vw_overridden_keys)),
            vw_overridden_keys_buf if len(vw_overridden_keys_buf) else None,
            vw_overridden_values_buf if len(vw_overridden_values_buf) else None,
            POINTER(c_char_p)(create_string_buffer((self.__facerec_conf_dir + recognizer_config.config_filepath).encode())),
            c_int32(len(rec_overridden_keys)),
            rec_overridden_keys_buf if len(rec_overridden_keys_buf) else None,
            rec_overridden_values_buf if len(rec_overridden_values_buf) else None,
            c_int32(params.streams_count),
            c_int32(params.processing_threads_count),
            c_int32(params.matching_threads_count),
            c_uint32(params.short_time_identification_enabled),
            c_float(params.short_time_identification_distance_threshold),
            c_float(params.short_time_identification_outdate_time_seconds),
            c_int32(params.age_gender_estimation_threads_count),
            c_int32(params.emotions_estimation_threads_count),
            exception)

        check_exception(exception, self._dll_handle)

        return VideoWorker(self._dll_handle, c_void_p(vw_impl))

    ##
    # \~English
    #    \brief Loads a RawSample object that was saved with the RawSample.save_without_image member function.
    #      If you transformed the image in some way, you can set the parameters of coordinate space transformation:
    #      (x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
    #      that will be applied to the information abour face position.
    #      Note that you must set the same transformation only once - either during serialization or during deserialization,
    #      otherwise transformation will be applied twice, which is wrong.
    #      The format is platform-independent.
    #      Thread-safe.
    #
    #    \param[in] binary_stream
    #      Input stream object.
    #      The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.
    #
    #    \param[in] image
    #      Image.
    #
    #    \param[in] space_translation_x
    #      Coordinate space transform parameter.
    #
    #    \param[in] space_translation_y
    #      Coordinate space transform parameter.
    #
    #    \param[in] space_scale
    #      Coordinate space transform parameter.
    #
    #    \return Loaded RawSample object.
    #
    # \~Russian
    #    \brief Загрузить объект RawSample, сохраненный с помощью метода RawSample.save_without_image.
    #      В случае, если над изображением была выполнена трансформация,
    #      вы можете указать параметры преобразования пространства координат:
    #      (x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
    #      которое будет применено к информации о позиции лица.
    #      Заметьте, что параметры одного и того же преобразования нужно передавать только один раз - либо
    #      при сериализации, либо при десериализации, иначе преобразование будет применено дважды, что неверно.
    #      Формат платформонезависимый.
    #      Потокобезопасный.
    #
    #    \param[in] binary_stream
    #      Объект потока ввода.
    #      Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
    #
    #    \param[in] image
    #      Изображение.
    #
    #    \param[in] space_translation_x
    #      Параметр преобразования пространства координат.
    #
    #    \param[in] space_translation_y
    #      Параметр преобразования пространства координат.
    #
    #    \param[in] space_scale
    #      Параметр преобразования пространства координат.
    #
    #    \return Загруженный объект RawSample.
    def load_raw_sample_without_image(self, binary_stream: BytesIO, image: bytes, space_translation_x: float = 0,
                                      space_translation_y: float = 0, space_scale: float = 1) -> RawSample:
        exception = make_exception()

        raw_sampl_impl = self._dll_handle.FacerecService_loadRawSampleWithoutImage_encoded_image(
            self._impl,
            py_object(binary_stream),
            read_func,
            c_char_p(image),
            len(image),
            c_double(space_translation_x),
            c_double(space_translation_y),
            c_double(space_scale),
            exception)

        check_exception(exception, self._dll_handle)

        return RawSample(self._dll_handle, c_void_p(raw_sampl_impl))

    ##
    #   \~English
    #      \brief Loads a RawSample object that was saved with the RawSample.save member function.
    #        The format is platform-independent.
    #        Thread-safe.
    #
    #      \param[in] binary_stream
    #        Input stream object.
    #        The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.
    #
    #      \return Loaded RawSample object.
    #
    #   \~Russian
    #      \brief Загрузить объект RawSample, сохраненный с помощью метода RawSample.save.
    #        Формат платформонезависимый.
    #        Потокобезопасный.
    #
    #      \param[in] binary_stream
    #        Объект потока ввода.
    #        Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
    #
    #      \return Загруженный объект RawSample.
    def load_raw_sample(self, binary_stream: BytesIO) -> RawSample:
        exception = make_exception()

        raw_sampl_impl = self._dll_handle.FacerecService_loadRawSample(
            self._impl,
            py_object(binary_stream),
            read_func,
            exception)

        check_exception(exception, self._dll_handle)

        return RawSample(self._dll_handle, c_void_p(raw_sampl_impl))
