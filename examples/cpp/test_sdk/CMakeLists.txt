cmake_minimum_required(VERSION 2.8)

set(name test_sdk)

project(${name})

include_directories(include)

add_executable(${name}
	src/common.cpp
	src/convert_config_format.cpp
	src/detection.cpp
	src/main.cpp
	src/processing.cpp
	src/recognition_test11.cpp
	src/recognition_test1N.cpp
	src/search_speed_test.cpp
	)

target_link_libraries(${name} pbio_cpp)

if(TARGET_OS_LINUX)
	target_link_libraries(${name} pthread)
endif()

install(TARGETS ${name} DESTINATION bin)
