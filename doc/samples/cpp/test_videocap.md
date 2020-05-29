# test_videocap

The program demonstrates face tracking and liveness estimation.

Startup parameters:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory

Example of launch from the *bin* directory:

Linux: `./test_videocap ../lib/libfacerec.so ../conf/facerec`  
Windows: `test_videocap facerec.dll ../conf/facerec`  

The result of face tracking, as well as the results of liveness estimation, are displayed in the console. Error messages, if any, are also displayed in the console.

Source code: [cpp/test_videocap/test_videocap.cpp](../../../examples/cpp/test_videocap/test_videocap.cpp)

The OpenCV library is required for build.
