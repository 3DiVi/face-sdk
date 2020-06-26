cmake_minimum_required(VERSION 2.8)

set(name demo)

project(${name})

add_executable(${name} demo.cpp)

target_link_libraries(${name} pbio_cpp opencv_videoio opencv_highgui opencv_imgcodecs opencv_imgproc)

install(TARGETS ${name} DESTINATION bin)
