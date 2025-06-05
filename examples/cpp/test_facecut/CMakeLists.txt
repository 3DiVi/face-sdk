cmake_minimum_required(VERSION 3.5)

set(name test_facecut)

project(${name})

add_executable(${name} test_facecut.cpp)

target_link_libraries(${name} pbio_cpp)

install(TARGETS ${name} DESTINATION bin)
