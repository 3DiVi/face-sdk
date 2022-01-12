# Face SDK python-api
`face_sdk_3divi` package allows you to use FaceSDK in Python language

## Requirements
Python 3.6 or higher is supported. Setuptools package must be installed before starting the installation.

## Install
Go to the python_api directory and run `pip3 install .`

## Platforms
You can use `face_sdk_3divi` package on Linux, Windows platforms

## Troubleshooting

| Error | Solution |
| ----- | -------- |
| `Assertion failed (Cannot open shared object file libtensorflow.so.2)` | Make sure the library file libtensorflow.so.2 is in the same directory as the libfacerec.so library you are using |
| `Assertion failed (Cannot open shared object file tensorflow.dll)` | Make sure the library file tensorflow.dll is in the same directory as the facerec.dll library you are using |

