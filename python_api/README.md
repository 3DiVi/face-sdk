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
| `OSError: libtensorflow.so.2: cannot open shared object file: No such file or directory` | You need to set the environment variable `LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/path/to/lib`, where `path/to/lib` is the path to the *lib* directory from the Face SDK  |
