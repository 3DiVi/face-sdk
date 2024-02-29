cmake_minimum_required(VERSION 2.8)

project(face_sdk_cpp_demos)

set(TARGET_OS_LINUX ${UNIX} CACHE BOOL "target is linux")

find_package(OpenCV)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(pbio_cpp)
add_subdirectory(test_identify)
add_subdirectory(test_facecut)
add_subdirectory(test_filecap)
add_subdirectory(test_sdk)
add_subdirectory(test_calibration)
add_subdirectory(demo)
add_subdirectory(video_recognition_demo)
add_subdirectory(test_videocap)

if(NOT WITHOUT_PROCESSING_BLOCK)
    add_subdirectory(processing_block)
endif()
