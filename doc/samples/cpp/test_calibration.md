# test_calibration

A program for camera calibration, an example of using the `pbio::CameraCalibrator` object.

The program runs in two modes:

* `calibrate` – to perform a camera calibration
* `view` – to see the result of distortion correction on the basis of calibration results

Startup parameters in the calibration mode:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* webcam id or stream url
* desired image frame width
* desired image frame height
* `calibrate`
* pattern type (`acircles`, `circles` or `chessboard`)
* pattern width
* pattern height
* path to the file, in which you want to save the calibrated parameters

Example of launch from the *bin* directory in the `calibrate` mode:

Linux: `./test_calibration ../lib/libfacerec.so ../conf/facerec/ 0 640 480 calibrate acircles 4 11 ./camparam.bin`  
Windows: `test_calibration facerec.dll ../conf/facerec/ 0 640 480 calibrate acircles 4 11 ./camparam.bin`  

To perform the calibration, you need to print a calibration template (see [Camera Calibration and Correction of Distortion](../../development/camera_calibration.md)) and fix it on a flat surface, preferably black. We recommend you to use the asymmetrical circle pattern. Then move either the pattern or the camera to capture the pattern in different poses. The program will show the approximate coverage of the pattern pose space and draw tips about the required pattern poses. Then press the `c` key to perform calibration. This may take about a minute, during this time the program freezes.

If the calibration is successful, the program shows a reprojection error and performs correction of frame distortion. Use the `q` and `w` keys to change the distortion correction parameter alpha. Press the `s` key to save the calibrated parameters or capture more pattern poses and perform the calibration again.

Startup parameters in the `view` mode:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* webcam id or stream url
* desired image frame width
* desired image frame height
* `view`
* path to the file, in which the camera parameters were saved

Example of launch from the *bin* directory in the `view` mode:

Linux: `./test_calibration ../lib/libfacerec.so ../conf/facerec/ 0 640 480 view ./camparam.bin`  
Windows: `test_calibration facerec.dll ../conf/facerec/ 0 640 480 view ./camparam.bin`  

Use the `q` and `w` keys to change the distortion correction parameter alpha.

Source code: [cpp/test_calibration/test_calibration.cpp](../../../examples/cpp/test_calibration/test_calibration.cpp)

The OpenCV library is required for build.
