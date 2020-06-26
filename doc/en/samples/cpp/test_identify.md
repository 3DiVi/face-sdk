# test_identify

The program demonstrates creation, saving, loading and matching of templates.

The program operates in three modes:

* **enrollment** – face detection in a group of images, creation of a template for each face, and saving the templates to a file.
* **identification** – face detection in one image, template creation, and matching of the template with every template created in the enrollment mode and loaded from the file.
* **verification** – detection of faces in two images, creation of two templates and their matching.

Launch parameters in the **enrollment** mode:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* name of the `Recognizer` config file
* `enroll`
* path to the directory with images (e.g. *bin/set1*)
* path to a text file containing a list of image file names (e.g. `bin/set1/list.txt`)
* path for saving templates to a file

Launch parameters in the **identification** mode:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* name of the `Recognizer` config file
* `identify`
* path to the image file
* path to the file with templates that was created in the enrollment mode

Launch parameters in the **verification** mode:

* path to the `libfacerec.so` or `facerec.dll` library file
* path to the *conf/facerec* directory
* name of the `Recognizer` config file
* `verify`
* path to the first image file
* path to the second image file

Example of launch from the *bin* directory in the identification mode:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method7v7_recognizer.xml enroll set1 set1/list.txt templates.bin`
* Windows: `test_identify facerec.dll ../conf/facerec method7v7_recognizer.xml enroll set1 set1/list.txt templates.bin`

Example of launch from the *bin* directory in the enrollment mode:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method7v7_recognizer.xml identify set2/01100.jpg templates.bin`
* Windows: `test_identify facerec.dll ../conf/facerec method7v7_recognizer.xml identify set2/01100.jpg templates.bin`

Example of launch from the *bin* directory in the verification mode:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method7v7_recognizer.xml verify set1/01100.jpg set2/01100.jpg`
* Windows: `test_identify facerec.dll ../conf/facerec method7v7_recognizer.xml verify set1/01100.jpg set2/01100.jpg`

Work progress and matching results are displayed in the console. Error messages, if any, are also displayed in the console.  

Source code: [test_identify.cpp](../../../../examples/cpp/test_identify/test_identify.cpp)

No additional libraries are required for build.
