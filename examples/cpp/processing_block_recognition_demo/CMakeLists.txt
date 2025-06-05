cmake_minimum_required(VERSION 3.5)

set(name processing_block_recognition_demo)

project(${name})

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(${name} main.cpp)

set(LIBS pbio_cpp ${OpenCV_LIBS})

target_link_libraries( ${name} ${LIBS})

install(TARGETS ${name} DESTINATION bin)
