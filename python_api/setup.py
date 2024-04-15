from setuptools import setup, find_packages
from os import path

__version__ = '3.21.0'
__author__ = '3DiVi'
__url__ = 'https://face.3divi.com/'


if __name__ == '__main__':
    _dir = path.abspath(path.dirname(__file__))
    with open(path.join(_dir, 'README.md'), encoding='utf-8') as f:
        long_description = f.read()

    setup(
        name="face_sdk_3divi",
        version=__version__,
        author=__author__,
        url=__url__,
        description="Package allows you to use FaceSDK in python language",
        long_description=long_description,
        long_description_content_type='text/markdown',
        packages=find_packages(),
        python_requires='>=3.6',
        install_requires=["numpy>=1.16.2,<=1.23.5", "opencv-python<=4.6.0.66,>4", "multipledispatch"],
    )
