TEMPLATE = app
CONFIG += qt uic resources c++11

QT += core gui widgets

# Set path to FaceSDK root directory
FACE_SDK_PATH =
isEmpty(FACE_SDK_PATH) {
	error("Empty path to Face SDK")
}

INCLUDEPATH += $${FACE_SDK_PATH}/include

DEFINES += FACE_SDK_PATH=\\\"$$FACE_SDK_PATH\\\"

unix: LIBS += -ldl

SOURCES = main.cpp
