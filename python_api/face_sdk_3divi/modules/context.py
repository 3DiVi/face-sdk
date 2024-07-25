##
#  \file context.py
#  \~English
#     \brief Context is an interface object for storing data and interacting with methods from the Processing Block API.
#  \~Russian
#     \brief Context - интерфейсный объект для хранения данных и взаимодействия с методами из Processing Block API.

from enum import Enum
from multipledispatch import dispatch
from ctypes import c_char_p, c_void_p
from ctypes import c_int32, c_int64, c_uint64, c_bool, c_double, POINTER, c_ubyte, string_at

from .exception_check import check_processing_block_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle

## @defgroup PythonAPI
#  @{
## @defgroup Context
#  @{

class ContextFormat(Enum):
    FORMAT_BGR = 0
    FORMAT_RGB = 1
    FORMAT_BGRA8888 = 2
    FORMAT_YUV_NV21 = 3
    FORMAT_YUV_NV12 = 4

##
#  \~English
#     \brief Interface object for the container is Context.
#  \~Russian
#     \brief Интерфейсный объект для контейнера-Context.
class Context(ComplexObject):

    def __init__(self, handle: DllHandle, the_impl=None):
        self.__weak_ = False
        self.data = None
        if the_impl is None:
            exception = make_exception()

            the_impl = c_void_p(handle.create(exception))

            check_processing_block_exception(exception, handle)

        super(Context, self).__init__(handle, the_impl)

    @classmethod
    def from_image(cls, handle: DllHandle, data: bytes):
        exception = make_exception()

        the_impl = c_void_p(handle.create_from_encoded_image(data, len(data), exception))

        check_processing_block_exception(exception, handle)

        return cls(handle, the_impl)

    @classmethod
    def from_frame(cls, handle: DllHandle, data: bytes, width: int, height: int, format: ContextFormat, base_angle: int):
        exception = make_exception()

        the_impl = c_void_p(handle.create_from_frame(data, width, height, format.value, base_angle, exception))

        check_processing_block_exception(exception, handle)

        return cls(handle, the_impl)

    @classmethod
    def from_json_file(cls, handle: DllHandle, path: str):
        exception = make_exception()

        the_impl = c_void_p(handle.create_from_json_file(c_char_p(bytes(path, "ascii")), exception))

        check_processing_block_exception(exception, handle)

        return cls(handle, the_impl)

    def __del__(self):
        if not self.__weak_:
            exception = make_exception()

            self._dll_handle.destroy(self._impl, exception)

            check_processing_block_exception(exception, self._dll_handle)

    def __call__(self, ctx):
        self.parser(ctx)

    @dispatch(int)
    def __getitem__(self, key):
        return self.__getByIndex(key)

    @dispatch(str)
    def __getitem__(self, key):
        return self.__getOrInsertByKey(key)

    ##
    # \~English
    #    \brief Changing the value by key.
    #    \param[in] key is the key for indexing.
    #    \param[in] value new value
    #    \return self.
    # \~Russian
    #    \brief Изменение значения по ключу.
    #    \param[in] key ключ для индексации.
    #    \param[in] value новое значение.
    #    \return self.
    def __setitem__(self, key, value):
        return self.__getOrInsertByKey(key).parser(value)

    ##
    # \~English
    #    \brief Get the size of the container.
    #    \return the size of the container.
    # \~Russian
    #    \brief Получить размер контейнера.
    #    \return размер контейнера.
    def __len__(self):
        return self.__getLength()

    def __iter__(self):
        self.n = 0
        return self

    def __next__(self):
        if self.n >= self.__getLength():
            raise StopIteration

        result = self.__getByIndex(self.n)
        self.n += 1
        return result

    def __contains__(self, key: str):
        return self.contains(key)

    def set_weak(self, weak):
        self.__weak_ = weak
        return self

    ##
    # \~English
    #    \brief adds a object to the container.
    # \~Russian
    #    \brief добавляет объект в контейнер.
    def push_back(self, data):
        if isinstance(data, Context):
            self.__pushBack(data)
        else:
            ctx = Context(self._dll_handle)
            ctx.parser(data)
            self.__pushBack(ctx)

    def __setStr(self, value: str):
        exception = make_exception()

        self._dll_handle.putStr(self._impl, c_char_p(bytes(value, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)

    def __getStr(self):
        exception = make_exception()

        buff = c_char_p()
        str1 = self._dll_handle.getStr(self._impl, buff, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return str(str1, "ascii")

    def __setDouble(self, value: float):
        exception = make_exception()

        self._dll_handle.putDouble(self._impl, c_double(value), exception)

        check_processing_block_exception(exception, self._dll_handle)

    def __getDouble(self):
        exception = make_exception()

        value = self._dll_handle.getDouble(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    def __setBool(self, value: bool):
        exception = make_exception()

        self._dll_handle.putBool(self._impl, c_bool(value), exception)

        check_processing_block_exception(exception, self._dll_handle)

    def __setDataPtr(self, value: bytes):
        exception = make_exception()

        self._dll_handle.putDataPtr(self._impl, c_char_p(value), c_uint64(len(value)), exception)
        check_processing_block_exception(exception, self._dll_handle)

    def __getDataPtr(self):
        exception = make_exception()

        result = self._dll_handle.getDataPtr(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return result


    ##
    # \~English
    #    \brief get a string of bytes from the container.
    #    \param[in] size the size of the byte string.
    #    \return a string of bytes.
    # \~Russian
    #    \brief получить строку байтов из контейнера.
    #    \param[in] size размер строки байтов.
    #    \return строку байтов.
    def get_bytes(self, size):
        byte_ptr = self.__getDataPtr()
        return string_at(byte_ptr, size)

    ##
    # \~English
    #    \brief get a string of bytes from the pointer.
    #    \param[in] byte_ptr pointer.
    #    \param[in] size the size of the byte string.
    #    \return a string of bytes.
    # \~Russian
    #    \brief получить строку байтов из указателя.
    #    \param[in] byte_ptr указатель.
    #    \param[in] size размер строки байтов.
    #    \return строку байтов.
    @staticmethod
    def get_bytes_from_ptr(byte_ptr: POINTER(c_ubyte), size: int):
        return string_at(byte_ptr, size)

    def __getBool(self):
        exception = make_exception()

        value = self._dll_handle.getBool(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    def __setLong(self, value: int):
        exception = make_exception()

        self._dll_handle.putLong(self._impl, c_int64(value), exception)

        check_processing_block_exception(exception, self._dll_handle)

    def __getLong(self):
        exception = make_exception()

        value = self._dll_handle.getLong(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    def __getLength(self):
        exception = make_exception()

        value = self._dll_handle.getLength(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    def getStrSize(self):
        exception = make_exception()

        value = self._dll_handle.getStrSize(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    def __getOrInsertByKey(self, key: str):
        exception = make_exception()

        new_impl = self._dll_handle.getOrInsertByKey(self._impl, c_char_p(bytes(key, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)

        return Context(self._dll_handle, c_void_p(new_impl)).set_weak(True)

    def __getByKey(self, key: str):
        exception = make_exception()

        new_impl = self._dll_handle.getByKey(self._impl, c_char_p(bytes(key, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)

        return Context(self._dll_handle, c_void_p(new_impl)).set_weak(True)

    def __getByIndex(self, key: int):
        exception = make_exception()

        new_impl = self._dll_handle.getByIndex(self._impl, c_int32(key), exception)

        check_processing_block_exception(exception, self._dll_handle)

        return Context(self._dll_handle, c_void_p(new_impl)).set_weak(True)

    ##
    # \~English
    #    \brief copies the container-Context to the new object.
    #    \return a new container object-Context.
    # \~Russian
    #    \brief копирует контейнер-Context в новый объект.
    #    \return новый объект контейнера-Context.
    def clone(self):
        exception = make_exception()

        impl = self._dll_handle.clone(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)

        return Context(self._dll_handle, c_void_p(impl))

    ##
    # \~English
    #    \brief clears the contents of the container-Context.
    # \~Russian
    #    \brief очищает содержимое контейнера-Context.
    def clear(self):
        exception = make_exception()

        self._dll_handle.clear(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief deletes the contents of the container-Context stored by key.
    #    \param[in] key.
    # \~Russian
    #    \brief удаляет содержимое контейнера-Context хранящиеся по ключу.
    #    \param[in] key.
    def erase(self, key: str):
        exception = make_exception()

        self._dll_handle.clear(self._impl, c_char_p(bytes(key, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief allocates memory for num elements in the array.
    #    \param[in] size - the size of the array.
    # \~Russian
    #    \brief выделяет память под num элементов в массиве.
    #    \param[in] size - размер массива.
    def reserve(self, size: int):
        exception = make_exception()

        self._dll_handle.reserve(self._impl, c_uint64(size), exception)

        check_processing_block_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief checks the existence of an element by a specific key
    #    \param[in] key
    #    \return True if the element exists
    # \~Russian
    #    \brief проверяет существование элемента по определённому ключу
    #    \param[in] key
    #    \return True если элемент существует
    def contains(self, key: str):
        exception = make_exception()

        result = self._dll_handle.contains(self._impl, c_char_p(bytes(key, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief compares two Context objects
    #    \param[in] other - container-Context
    #    \return True if the objects are the same
    # \~Russian
    #    \brief сравнивает два объекта Context
    #    \param[in] other - контейнер-Context
    #    \return True если объекты одинаковые
    def compare(self, other):
        assert isinstance(other, Context), "the object does not belong to the Context type"
        exception = make_exception()

        result = self._dll_handle.compare(self._impl, other._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)

        return result

    def __pushBack(self, data):
        exception = make_exception()

        self._dll_handle.pushBack(self._impl, data._impl, c_bool(True), exception)

        check_processing_block_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief converts the container-Context to dict
    #    \return dict
    # \~Russian
    #    \brief преобразует контейнер-Context в dict
    #    \return dict
    def to_dict(self):
        if self.is_array():
            return [self[i].to_dict() for i in range(len(self))]

        if self.is_object():
            return {key: self[key].to_dict() for key in self.keys()}

        return self.get_value()


    @dispatch(dict)
    def parser(self, ctx: dict):
        for key in ctx.keys():
            self.__getOrInsertByKey(key)(ctx[key])

    @dispatch(list)
    def parser(self, ctx: list):
        for value in ctx:
            ctt = Context(self._dll_handle)
            ctt.parser(value)
            self.__pushBack(ctt)

    @dispatch(str)
    def parser(self, ctx: str):
        self.__setStr(ctx)

    @dispatch(int)
    def parser(self, ctx: int):
        self.__setLong(ctx)

    @dispatch(float)
    def parser(self, ctx: float):
        self.__setDouble(ctx)

    @dispatch(bool)
    def parser(self, ctx: bool):
        self.__setBool(ctx)

    @dispatch(bytes)
    def parser(self, ctx: bytes):
        self.__setDataPtr(ctx)

    @dispatch(object)
    def parser(self, ctx):
        assert isinstance(ctx, Context), "the object does not belong to the base types or Context"
        if ctx is not self:
            if self.__weak_:
                exception = make_exception()
                self._dll_handle.copy(ctx._impl, self._impl, exception)
                check_processing_block_exception(exception, self._dll_handle)

    def __get_keys(self) -> list:
        exception = make_exception()

        cout_keys = self.__getLength()
        buf = POINTER(c_char_p)

        p_value_array = self._dll_handle.getKeys(self._impl, c_uint64(cout_keys), exception)
        check_processing_block_exception(exception, self._dll_handle)

        result = [buf(i)[0].decode() for i in p_value_array[0]]

        for i in p_value_array[0]:
            self._dll_handle.freePtr(i)
        self._dll_handle.freePtr(p_value_array[0])

        return result

    ##
    # \~English
    #    \brief returns a list of keys in the container-Context
    #    \return key list
    # \~Russian
    #    \brief возвращает список ключей в контейнере-Context
    #    \return список ключей
    def keys(self) -> list:
        return self.__get_keys()

    ##
    # \~English
    #    \brief checks if there are no elements in the container
    #    \return True if the container is empty
    # \~Russian
    #    \brief проверяет нет ли в контейнере элементов
    #    \return True если контейнер пуст
    def is_none(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isNone(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is an array
    #    \return True if the container is an array
    # \~Russian
    #    \brief проверяет является ли контейнере массивом
    #    \return True если контейнер является массивом
    def is_array(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isArray(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is an object
    #    \return True if the container is an object
    # \~Russian
    #    \brief проверяет является ли контейнер объектом
    #    \return True если контейнер является объектом
    def is_object(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isObject(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is a bool type value
    #    \return True if the container is a bool type value
    # \~Russian
    #    \brief проверяет является ли контейнер значением типа bool
    #    \return True если контейнер является значением типа bool
    def is_bool(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isBool(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is a long type value
    #    \return True if the container is a long type value
    # \~Russian
    #    \brief проверяет является ли контейнер значением типа long
    #    \return True если контейнер является значением типа long
    def is_long(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isLong(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is a double type value
    #    \return True if the container is a double type value
    # \~Russian
    #    \brief проверяет является ли контейнер значением типа double
    #    \return True если контейнер является значением типа double
    def is_double(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isDouble(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is a string type value
    #    \return True if the container is a string type value
    # \~Russian
    #    \brief проверяет является ли контейнер значением типа string
    #    \return True если контейнер является значением типа string
    def is_string(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isString(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief checks whether the container is a pointer to the data
    #    \return True if the container is a pointer to the data
    # \~Russian
    #    \brief проверяет является ли контейнер указателем на данные
    #    \return True если контейнер является указателем на данные
    def is_data_ptr(self) -> bool:
        exception = make_exception()

        value = self._dll_handle.isDataPtr(self._impl, exception)

        check_processing_block_exception(exception, self._dll_handle)
        return value

    ##
    # \~English
    #    \brief returns the value contained in the container
    #    \return value
    # \~Russian
    #    \brief возвращает значение содержащиеся в контейнере
    #    \return значение
    def get_value(self):
        if self.is_none():
            return None
        if self.is_bool():
            return self.__getBool()
        if self.is_string():
            return self.__getStr()
        if self.is_long():
            return self.__getLong()
        if self.is_double():
            return self.__getDouble()
        if self.is_data_ptr():
            return self.__getDataPtr()

        return None

    ##
    # \~English
    #    \brief saves the contents of the container to a json file
    #    \param[in] path - the path to the file
    # \~Russian
    #    \brief сохраняет содержимое контейнера в json файл
    #    \param[in] path - путь до файла
    def save_to_json_file(self, path: str):
        exception = make_exception()

        self._dll_handle.save_to_json_file(self._impl, c_char_p(bytes(path, "ascii")), exception)

        check_processing_block_exception(exception, self._dll_handle)