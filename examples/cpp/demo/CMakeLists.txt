cmake_minimum_required(VERSION 3.5)

set(name demo)

project(${name})

set(LIBS pbio_cpp opencv_videoio opencv_highgui opencv_imgcodecs opencv_imgproc)

add_executable(${name} main.cpp)

target_link_libraries(${name} ${LIBS})

install(TARGETS ${name} DESTINATION bin)
