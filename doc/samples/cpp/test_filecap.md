# test_filecap

The program demonstrates face detection and saving and loading of `pbio::RawSample`.

Startup parameters:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* capturer config file name
* paths to the input image files

Example of launch from the *bin* directory:

Linux: `./test_filecap ../lib/libfacerec.so ../conf/facerec common_capturer4.xml set1/01100.jpg`  
Windows: `test_filecap facerec.dll ../conf/facerec common_capturer4.xml set1/01100.jpg`  

Face detection result is displayed in a window. The console displays the coordinates and orientations of the detected faces, as well as the paths, which are used to store the samples. Error messages, if any, are also printed in the console.

Source code: [cpp/test_filecap/test_filecap.cpp](../../../examples/cpp/test_filecap/test_filecap.cpp)

The OpenCV library is required for build.
