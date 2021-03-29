#  \file Recognizer.py
#  \~English
#     \brief Recognizer - Interface object for creating and matching templates.
#  \~Russian
#     \brief Recognizer - Интерфейсный объект для создания и сравнения шаблонов.

from ctypes import c_int64, c_void_p,  c_float, py_object, c_double, byref, c_int8, c_int32

from io import BytesIO
from enum import Enum
from typing import List

from .exception_check import check_exception, make_exception
from .wrap_funcs import write_func, read_func
from .templates_index import TemplatesIndex
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from .template import Template
from .error import Error
from . import get_repr

## @defgroup PythonAPI
#  @{
## @defgroup Recognizer
#  @{

##
# \~English
#    \brief Interface object for creating and matching templates.
# \~Russian
#    \brief Интерфейсный объект для создания и сравнения шаблонов.
class MatchResult:

    #  \~English
    #     \brief Distance between the templates.
    #  \~Russian
    #     \brief Расстояние между шаблонами.
    distance: float

    #  \~English
    #     \brief
    #       False acceptance rate corresponding to the distance value taken as a threshold
    #       at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
    #  \~Russian
    #     \brief
    #       Значение FAR (false acceptance rate), соответствующее расстоянию distance, взятому в качестве порога
    #       на расширенном LFW-тесте (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
    fa_r: float

    #  \~English
    #     \brief
    #       False rejection rate corresponding to the distance value taken as a threshold
    #       at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
    #  \~Russian
    #     \brief
    #       Значение FRR (false rejection rate), соответствующее расстоянию distance, взятому в качестве порога
    #       на расширенном LFW тесте (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
    fr_r: float

    #  \~English
    #     \brief Score of templates similarity - real number from 0 to 1.
    #  \~Russian
    #     \brief Величина сходства шаблонов - действительное число от 0 до 1.
    score: float

    def __init__(self, distance: float, fa_r: float, fr_r: float, score: float):
        self.distance = distance
        self.fa_r = fa_r
        self.fr_r = fr_r
        self.score = score

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Result of searching the Template query in the TemplatesIndex.
# \~Russian
#    \brief Результат поиска запросного шаблона (Template) в индексе (TemplatesIndex).
class SearchResult:

    #  \~English
    #     \brief Index in the TemplatesIndex.
    #  \~Russian
    #     \brief Порядковый номер шаблона в индексе.
    i: int

    #  \~English
    #     \brief Result of matching the query Template with the i-th Template from the TemplatesIndex.
    #  \~Russian
    #     \brief Результат сравнения запросного шаблона с i-м шаблоном индекса.
    match_result: MatchResult

    def __init__(self, i: int, match_result: MatchResult):
        self.i = i
        self.match_result = match_result

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Types of search acceleration.
# \~Russian
#    \brief Типы ускорения поиска.
class SearchAccelerationType(Enum):

    #  \~English
    #     \brief No acceleration, a standard line search.
    #       The result is identical to N calls of Recognizer.verify_match.
    #  \~Russian
    #     \brief Без ускорения, стандартный линейный поиск.
    #       Результат идентичен N вызовам Recognizer.verify_match.
    NO_SEARCH_ACCELERATION = 0

    #  \~English
    #     \brief Heuristically accelerated linear search.
    #       Heuristics reduces TAR (true acceptance rate) on
    #       the 1:N test by no more than 0.1%
    #  \~Russian
    #     \brief Эвристически ускоренный линейный поиск.
    #       Эвристика уменьшает TAR (true acceptance rate) на
    #       тесте 1:N не более чем на 0.1%
    SEARCH_ACCELERATION_1 = 1


##
# \~English
#    \brief Interface object for creating and matching templates.
# \~Russian
#    \brief Интерфейсный объект для создания и сравнения шаблонов.
class Recognizer(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(Recognizer, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Create a template.
    #
    #    \param[in] sample
    #      Face sample.
    #      At the moment, all methods can only be used with frontal samples.
    #
    #    \return Created template.
    #
    # \~Russian
    #    \brief Создать шаблон.
    #
    #    \param[in] sample
    #      Образец лица.
    #      На данный момент все методы могут работать только с фронтальными образцами.
    #
    #    \return Созданный шаблон.
    def processing(self, sample: RawSample) -> Template:
        exception = make_exception()

        result_impl = self._dll_handle.Recognizer_processing(
            self._impl,
            sample._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return Template(self._dll_handle, c_void_p(result_impl))

    ##
    # \~English
    #    \brief Compare two templates.
    #      The order of templates does not matter.
    #      Only the templates that were created with the same method
    #      (i.e. with the same ini_file) can be loaded.
    #
    #     \param[in] template1
    #       %Template created by the same method.
    #
    #     \param[in] template2
    #       %Template created by the same method.
    #
    #     \return Result of the matching.
    #
    # \~Russian
    #    \brief Сравнить два шаблона.
    #      Порядок шаблонов не важен.
    #      Сравнивать можно только шаблоны, созданные этим же методом
    #      (т.е. с таким же конфигурационным файлом).
    #
    #    \param[in] template1
    #      Шаблон, созданный таким же методом.
    #
    #    \param[in] template2
    #      Шаблон, созданный таким же методом.
    #
    #    \return Результат сравнения.
    def verify_match(self, template1: Template,
                     template2: Template):
        exception = make_exception()

        distance = c_double()
        fa_r = c_double()
        fr_r = c_double()
        score = c_double()

        self._dll_handle.Recognizer_verifyMatch_v2(
            self._impl,
            template1._impl,
            template2._impl,
            byref(distance),
            byref(fa_r),
            byref(fr_r),
            byref(score),
            exception)

        check_exception(exception, self._dll_handle)

        return MatchResult(distance.value, fa_r.value, fr_r.value, score.value)

    ##
    # \~English
    #    \brief Create the TemplatesIndex for quick search.
    #      The total size of all indexes is limited by the license.
    #
    #    \param[in] templates
    #      Vector of templates for creating an index.
    #
    #    \param[in] search_threads_count
    #      Count of threads that will be used while searching in this index.
    #
    #    \param[in] reserve_queries_count
    #      Integer passed to TemplatesIndex.reserve_search_memory, which is called with the created index.
    #
    #    \return Created index.
    #
    # \~Russian
    #    \brief Создать индекс (TemplatesIndex) для выполнения быстрого поиска.
    #      Суммарный размер всех индексов ограничен лицензией.
    #
    #    \param[in] templates
    #      Вектор шаблонов для построения индекса.
    #
    #    \param[in] search_threads_count
    #      Количество потоков для использования во время поиска в этом индексе.
    #
    #    \param[in] reserve_queries_count
    #      Целое число, передаваемое в метод TemplatesIndex.reserve_search_memory, вызванный от созданного индекса.
    #
    #    \return Созданный индекс.
    def create_index(self, templates: List[Template],
                     search_threads_count: int) -> TemplatesIndex:
        templates_impls = (c_void_p * len(templates))()
        for index, template in enumerate(templates):
            templates_impls[index] = template._impl

        exception = make_exception()

        result_impl = self._dll_handle.Recognizer_createIndex(
            self._impl,
            len(templates),
            templates_impls,
            search_threads_count,
            exception)

        check_exception(exception, self._dll_handle)

        return TemplatesIndex(self._dll_handle, c_void_p(result_impl))

    ##
    # \~English
    #    \brief Get a method name.
    #      Thread-safe.
    #
    #    \return The name of the method.
    #
    # \~Russian
    #    \brief Получить имя метода.
    #      Потокобезопасный.
    #
    #    \return Имя метода.
    def get_method_name(self) -> str:
        name_stream = BytesIO()

        exception = make_exception()

        self._dll_handle.Recognizer_getMethodName(
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
    #    \brief Search for the k nearest Templates in the TemplatesIndex.
    #      The search time depends on k and the size of the TemplatesIndex.
    #
    #    \param[in] query_template
    #      The Template query.
    #
    #    \param[in] templates_index
    #      TemplatesIndex for search.
    #
    #    \param[in] k
    #      Count of the nearest templates for search.
    #
    #    \param[in] acceleration
    #      Acceleration type.
    #
    #    \return Vector with min(k, templates_index.size()) elements
    #      in ascending order of distance to the query_template.
    #
    # \~Russian
    #    \brief Поиск k ближайших шаблонов в индексе.
    #      Время поиска зависит от значения k и от размеров индекса.
    #
    #    \param[in] query_template
    #      Запросный шаблон.
    #
    #    \param[in] templates_index
    #      Индекс для поиска.
    #
    #    \param[in] k
    #      Количество ближайших шаблонов для поиска.
    #
    #    \param[in] acceleration
    #      Тип ускорения поиска.
    #
    #    \return Вектор с min(k, templates_index.size()) элементами
    #      в порядке возрастания расстояния до запросного шаблона.
    def search(self, queries_templates: List[Template],
               templates_index: TemplatesIndex,
               k: int,
               acceleration: SearchAccelerationType) -> List[List[SearchResult]]:
        queries_count = len(queries_templates)

        queries_impls = (c_void_p * queries_count)()
        for index, queries_template in enumerate(queries_templates):
            queries_impls[index] = queries_template._impl

        result_i = (c_int64 * (k * queries_count))()
        result_distance = (c_float * (k * queries_count))()
        result_far = (c_float * (k * queries_count))()
        result_frr = (c_float * (k * queries_count))()
        result_score = (c_float * (k * queries_count))()

        exception = make_exception()

        self._dll_handle.Recognizer_search_v2(
            self._impl,
            SearchAccelerationType(acceleration).value,
            queries_count,
            queries_impls,
            templates_index._impl,
            k,
            result_i,
            result_distance,
            result_far,
            result_frr,
            result_score,
            exception)

        check_exception(exception, self._dll_handle)

        result = list()
        for i in range(queries_count):
            result.append(list())
            for j in range(k):
                h = i * k + j

                result[i].append(SearchResult(result_i[h],
                                              MatchResult(result_distance[h], result_far[h], result_frr[h],
                                                          result_score[h])))

        return result

    ##
    # \~English
    #    \brief Load the template.
    #      The format is platform-independent.
    #      Only the templates that were created with the same method
    #      (i.e. with the same ini_file) can be loaded.
    #
    #    \param[in] binary_stream
    #      Input stream object.
    #      The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.
    #
    #    \return Loaded template.
    #
    # \~Russian
    #    \brief Загрузить шаблон.
    #      Формат платформонезависимый.
    #      Загружать можно только шаблоны, созданные этим же методом
    #      (т.е. с таким же конфигурационным файлом).
    #
    #    \param[in] binary_stream
    #      Объект потока.
    #      Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
    #
    #    \return Загруженный шаблон.
    def load_template(self, binary_stream: BytesIO) -> Template:
        exception = make_exception()

        result_impl = self._dll_handle.Recognizer_loadTemplate(
            self._impl,
            py_object(binary_stream),
            read_func,
            exception)

        check_exception(exception, self._dll_handle)

        return Template(self._dll_handle, c_void_p(result_impl))

    ##
    # \~English
    #    \brief Choose templates set that best represent original templates.
    #
    #    \param[in] set_size
    #      Required set size.
    #
    #    \param[in] templates
    #      Original templates.
    #
    #    \param[in] inviolable_templates_indexes
    #      Indexes of templates, required to be included in the result set.
    #
    #    \return Indexes of templates that make up the result set.
    #
    # \~Russian
    #    \brief Выбрать набор шаблонов, представляющий оригинальные шаблоны наилучшим образом.
    #
    #    \param[in] set_size
    #      Требуемый размер набора.
    #
    #    \param[in] templates
    #      Оригинальные шаблоны.
    #
    #    \param[in] inviolable_templates_indexes
    #      Индексы шаблонов, которых необходимо включить в набор.
    #
    #    \return Индексы шаблонов, составляющих результирующий набор.
    def choose_representative_templates_set(self, set_size: int,
                                            templates: [Template],
                                            inviolable_templates_indexes: [int] = None) -> [int]:
        templates_impls = (c_void_p * len(templates))()
        for index, template in enumerate(templates):
            templates_impls[index] = template._impl
        inviolable_templates_indexes = inviolable_templates_indexes or []

        inv_flags = (c_int8 * len(templates))()
        for i in inviolable_templates_indexes:
            if i >= len(templates):
                raise Error(0x63156958, "bad index in chooseRepresentativeTemplatesSet")
            inv_flags[i] = 1

        result = (c_int32 * set_size)()
        exception = make_exception()
        self._dll_handle.Recognizer_chooseRepresentativeTemplatesSet(
            self._impl,
            templates_impls,
            inv_flags,
            len(templates),
            result,
            set_size,
            exception
        )
        check_exception(exception, self._dll_handle)
        return [i for i in result]
