# demo

The program demonstrates tracking, detection and cropping of faces, detection of anthropometric points and angles, as well as estimation of face quality, age and gender, emotions, and liveness (by processing an RGB image from your camera).

To make a quick run of the demo with default parameters on Windows, go to the *bin* folder and double-left-click `demo.exe`.

<p align="center">
<img width="180" src="../../img/cpp_demo_exe.png"><br>
<b>Location of C++ demo</b>
</p>

In the upper left corner of the demo you can see the Face SDK components, which can be turned on and off with the left mouse click:

* **rectangles** – face rectangle
* **angles** – angles
* **quality** – image quality
* **liveness** – liveness
* **age and gender** – age and gender
* **base cut, full cut, token cut** – type of face cropping (basic, full frontal, token frontal)
* **points** – anthropometric points
* **face quality** – face quality
* **angles vectors** – vector angles
* **emotions** – emotions

<p align="center">
<img width="600" src="../../img/demo_cpp.png"><br>
<b>Running demo.exe</b>
</p>

Error messages (if any) are printed in the console.  

You can also run `demo.exe` specifying some parameters (for example, if you have an online license).  
Startup parameters:

* `capturer_conf` – path to the capturer config file (learn more about types of config files in [Capturer Class Reference](../../development/face_capturing.md#capturer-class-reference))
* `config_dir` – path to the *conf/facerec* directory
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `license_dir` – path to the directory with a license; provide this parameter if you need to change a default directory license

Source code: [demo.cpp](../../../examples/cpp/demo/demo.cpp)

The OpenCV library is required for build.
