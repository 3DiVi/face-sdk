cmake_minimum_required(VERSION 2.8)

set(name test_identify)

project(${name})

add_executable(${name} test_identify.cpp)

target_link_libraries(${name} pbio_cpp)

install(TARGETS ${name} DESTINATION bin)
