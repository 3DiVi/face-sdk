cmake_minimum_required(VERSION 3.5)

set(name demo)

project(${name})

set(LIBS pbio_cpp opencv_videoio opencv_highgui opencv_imgcodecs opencv_imgproc)

add_executable(${name} demo.cpp)

if (WIN32 OR (UNIX AND NOT APPLE))
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		add_definitions(-DPROCESSING_BLOCK_API)
	endif()
endif()

target_link_libraries(${name} ${LIBS})

install(TARGETS ${name} DESTINATION bin)
