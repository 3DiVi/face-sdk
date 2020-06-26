cmake_minimum_required(VERSION 2.8)

set(name test_filecap)

project(${name})

add_executable(${name} test_filecap.cpp)

target_link_libraries(${name} pbio_cpp opencv_highgui opencv_imgcodecs opencv_imgproc)

install(TARGETS ${name} DESTINATION bin)
