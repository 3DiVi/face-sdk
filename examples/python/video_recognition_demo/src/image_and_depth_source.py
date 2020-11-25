from face_sdk_3divi import DepthMapRaw

from ctypes import c_ushort


class ImageAndDepth:

	color_image: list

	color_width: int

	color_stride_in_bytes: int

	color_format: int

	depth_image: list

	image_timestamp_microsec: int

	depth_timestamp_microsec: int

	depth_opts: DepthMapRaw

	def __init__(self):
		self.color_image = []
		self.color_width = 0
		self.color_height = 0
		self.color_stride_in_bytes = 0
		self.color_format = 0
		self.depth_image = []
		self.image_timestamp_microsec = 0
		self.depth_timestamp_microsec = 0
		self.depth_opts = DepthMapRaw()

	def make_dmr(self):
		assert self.depth_image is not None

		seq = c_ushort * len(self.depth_image)

		r = self.depth_opts
		r.depth_data = seq(*self.depth_image)
		return r


class ImageAndDepthSource:

	def __init__(self):
		pass

	def get(self, data: ImageAndDepth):
		pass
