# test_facecut

The program demonstrates the detection and cropping of faces, as well as estimation of the face image quality, sex and age.

Startup parameters:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* paths to the input image files

Example of launch from the bin directory:

Linux: `./test_facecut ../lib/libfacerec.so ../conf/facerec set1/01100.jpg`  
Windows: `test_facecut facerec.dll ../conf/facerec set1/01100.jpg`  

The results of face detection, coordinates of the detected faces, estimated quality, emotions, gender and age, as well as the paths used to save the cropped images are displayed in the console. Error messages, if any, are also printed in the console.

Source code: [cpp/test_facecut/test_facecut.cpp](../../../examples/cpp/test_facecut/test_facecut.cpp)

No additional libraries are required for build.
