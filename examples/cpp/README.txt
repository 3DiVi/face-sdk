===
Build on linux:

install g++, cmake
install opencv, or build it from source

 > cd <path to face sdk distr>/examples/cpp

 > mkdir build

 > cd build

if opencv is installed in your system:
 > cmake -DCMAKE_INSTALL_PREFIX=../../../ -DCMAKE_BUILD_TYPE=Release ..
or:
 > cmake -DCMAKE_INSTALL_PREFIX=../../../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path to opencv install dir> ..
if opencv is built from source

build binaries
 > make

install built binaries in the <path to face sdk distr>/bin directory
 > make install


===
Build on windows:

download opencv release from https://opencv.org/releases/ corresponding to your Visual Studio version or build from source

install cmake (download binary release from https://cmake.org/download/)

run cmake-gui

select <path to face sdk distr>/examples/cpp as a source code directory

select build directory wherever you want to generate build data

click Configure

select your Visual Studio version

click Finish

there can be a error about OpenCV not found
to fix it find variable OpenCV_DIR and set it value to the path where opencv was installed or downloaded
then click Configure again

click Generate

click Open Project

Visual Studio should be opened with solution, containing all examples project

the solution file (face_sdk_cpp_demos.sln) is located in build directory that you choosed earlier

to run copy built binaries and opencv dll files in the <path to face sdk distr>/bin directory