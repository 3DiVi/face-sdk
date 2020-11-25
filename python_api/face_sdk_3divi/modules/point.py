##
#  \file Point.py
#  \~English
#     \brief 2D point.
#  \~Russian
#     \brief 2D-точка.

from typing import Union
from .error import Error
from . import get_repr


##
# \~English
#    \brief 3D point.
# \~Russian
#    \brief 3D-точка.
class Point:

    #  \~English
    #     \brief X coordinate.
    #  \~Russian
    #     \brief Координата X.
    x: float

    #  \~English
    #     \brief Y coordinate.
    #  \~Russian
    #     \brief Координата Y.
    y: float

    #  \~English
    #     \brief Z coordinate.
    #  \~Russian
    #     \brief Координата Z.
    z: float

    def __init__(self, x: float, y: float, z: float = 0.0):
        self.x = x
        self.y = y
        self.z = z

    def __mul__(self, other):
        if isinstance(other, Point):
            return Point(self.x * other.x,
                         self.y * other.y,
                         self.z * other.z)
        elif isinstance(other, Union[int, float].__args__):
            return Point(self.x * other,
                         self.y * other,
                         self.z * other)
        else:
            raise Error(0x0057e780, "Wrong type for multiply operation")

    def __sub__(self, other):
        if isinstance(other, Point):
            return Point(self.x - other.x,
                         self.y - other.y,
                         self.z - other.z)
        elif isinstance(other, Union[int, float].__args__):
            return Point(self.x - other,
                         self.y - other,
                         self.z - other)
        else:
            raise Error(0x0057e780, "Wrong type for subtraction operation")

    def __add__(self, other):
        if isinstance(other, Point):
            return Point(self.x + other.x,
                         self.y + other.y,
                         self.z + other.z)
        elif isinstance(other, Union[int, float].__args__):
            return Point(self.x + other,
                         self.y + other,
                         self.z + other)
        else:
            raise Error(0x0057e780, "Wrong type for subtraction operation")

    def __repr__(self):
        return get_repr(self)

    def __iter__(self):
        return iter([self.x, self.y, self.z])

    def __len__(self):
        return 3

    def __getitem__(self, item):
        return [self.x, self.y, self.z][item]
