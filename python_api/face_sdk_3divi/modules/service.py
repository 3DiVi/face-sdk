from ctypes import CDLL, create_string_buffer
from sys import platform
from ctypes import c_char_p, c_void_p, POINTER

from .dll_handle import DllHandle
from .facerec_service import FacerecService
from .exception_check import check_exception, make_exception


class Service(FacerecService):

    def __init__(self,  dll_handle: CDLL, facerec_conf_dir: str, impl: c_void_p, path_to_dir: str):
        super(Service, self).__init__(dll_handle, facerec_conf_dir, impl)
        self.path_to_dir = path_to_dir

    @staticmethod
    def create_service(path_to_dir: str):
        lib_path = ""
        if platform == "linux" or platform == "linux2":
            lib_path = "/lib/libfacerec.so"
        elif platform == "win32":
            lib_path = "\\bin\\facerec.dll"

        dll_path = path_to_dir + lib_path
        facerec_conf_dir = path_to_dir + "/conf/facerec"

        dll_handle = DllHandle(CDLL(dll_path))

        exception = make_exception()

        the_impl = dll_handle.FacerecService_constructor2(
            POINTER(c_char_p)(create_string_buffer(facerec_conf_dir.encode())),
            None,
            POINTER(c_char_p)(create_string_buffer(dll_path.encode())),
            exception)

        check_exception(exception, dll_handle)
        return Service(dll_handle, facerec_conf_dir, c_void_p(the_impl), path_to_dir)

    def create_processing_block(self, ctx: dict):
        new_ctx = ctx
        new_ctx["@sdk_path"] = self.path_to_dir
        return FacerecService.create_processing_block(self, new_ctx)