cmake_minimum_required(VERSION 3.1)

set(name processing_block_demo)

project(${name})

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(${name} processing_block.cpp)

set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)

target_link_libraries( ${name} pbio_cpp ${OpenCV_LIBS})

install(TARGETS ${name} DESTINATION bin)
