cmake_minimum_required(VERSION 3.5)

set(name test_calibration)

project(${name})

add_executable(${name} test_calibration.cpp)

target_link_libraries(${name} pbio_cpp opencv_calib3d opencv_videoio opencv_highgui)

install(TARGETS ${name} DESTINATION bin)
