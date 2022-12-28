cmake_minimum_required(VERSION 2.8)

set(name video_recognition_demo)

project(${name})

add_executable(${name}
	src/main.cpp
	src/Worker.cpp
	src/Database.cpp
	src/StreamVideoCapture.cpp
	)

set(LIBS pbio_cpp opencv_highgui opencv_videoio opencv_imgcodecs opencv_imgproc)

target_link_libraries(${name} ${LIBS})

if(" ${CMAKE_SYSTEM_NAME} " STREQUAL " Linux ")
	target_link_libraries(${name} pthread)
endif()

install(TARGETS ${name} DESTINATION bin)
install(DIRECTORY samples/ DESTINATION bin USE_SOURCE_PERMISSIONS)
