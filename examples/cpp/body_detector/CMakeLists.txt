cmake_minimum_required(VERSION 3.1)

set(name body_detector)

project(${name})

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(${name} body_detector.cpp)

set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)

target_link_libraries( ${name} pbio_cpp ${OpenCV_LIBS})

install(TARGETS ${name} DESTINATION bin)
