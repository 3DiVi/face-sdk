cmake_minimum_required(VERSION 2.8)

set(name test_videocap)

project(${name})

add_executable(${name} test_videocap.cpp)

target_link_libraries(${name} pbio_cpp opencv_highgui opencv_videoio opencv_imgproc)

install(TARGETS ${name} DESTINATION bin)
