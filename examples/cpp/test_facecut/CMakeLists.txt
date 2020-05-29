cmake_minimum_required(VERSION 2.8)

set(name test_facecut)

project(${name})

add_executable(${name} test_facecut.cpp)

target_link_libraries(${name} pbio_cpp)

install(TARGETS ${name} DESTINATION bin)
