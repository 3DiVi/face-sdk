cmake_minimum_required(VERSION 3.5)

set(name processing_block_demo)

project(${name})

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(${name} processing_block.cpp)

set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)

set(LIBS pbio_cpp ${OpenCV_LIBS})

target_link_libraries( ${name} ${LIBS})

install(TARGETS ${name} DESTINATION bin)
