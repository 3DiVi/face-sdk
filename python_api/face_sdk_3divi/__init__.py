##
#  \~English
#    \brief Python interface.
#  \~Russian
#    \brief Python интерфейс.


from .modules.config import Config
from .modules.facerec_service import FacerecService
from .modules.capturer import Capturer
from .modules.depth_map_raw import DepthMapRaw
from .modules.ir_frame_raw import IRFrameRaw
from .modules.point import Point
from .modules.template import Template
from .modules.templates_index import TemplatesIndex
from .modules.error import Error
from .modules.recognizer import Recognizer
from .modules.service import Service
from .modules.context import Context, ContextFormat
from .modules.context_template import ContextTemplate
from .modules.dynamic_template_index import DynamicTemplateIndex
from .modules.processing_block import ProcessingBlock