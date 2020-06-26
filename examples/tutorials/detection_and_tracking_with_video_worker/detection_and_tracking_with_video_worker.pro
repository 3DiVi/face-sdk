TEMPLATE = app
CONFIG += qt uic resources c++11

QT += core gui
QT += multimedia multimediawidgets

# Set path to FaceSDK root directory
FACE_SDK_PATH =
isEmpty(FACE_SDK_PATH) {
	error("Empty path to Face SDK")
}

DEFINES += FACE_SDK_PATH=\\\"$$FACE_SDK_PATH\\\"

INCLUDEPATH += $${FACE_SDK_PATH}/include

unix: LIBS += -ldl

HEADERS += \
	mainwindow.h \
	viewwindow.h \
	drawfunction.h \
	facesdkparameters.h \
	qcameracapture.h \
	videoframe.h \
	worker.h

SOURCES = \
	main.cpp \
	mainwindow.cpp \
	viewwindow.cpp \
	drawfunction.cpp \
	qcameracapture.cpp \
	worker.cpp

FORMS += \
	mainwindow.ui \
	viewwindow.ui
