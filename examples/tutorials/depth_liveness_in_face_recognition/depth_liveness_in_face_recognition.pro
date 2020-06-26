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
INCLUDEPATH += $$FACE_SDK_PATH/examples/cpp/video_recognition_demo/src

DEFINES += DATABASE_PATH=\\\"$${_PRO_FILE_PWD_}/base\\\"

WITH_OPENNI2=1
#WITH_REALSENSE=1

isEmpty(WITH_OPENNI2): isEmpty(WITH_REALSENSE) {
	error("OpenNI2 or RealSense support should be enabled")
}

!isEmpty(WITH_OPENNI2): !isEmpty(WITH_REALSENSE) {
	error("OpenNI2 and RealSense support can't be enabled simultaneously")
}

!isEmpty(WITH_OPENNI2){

	OpenniDistr =
	isEmpty(OpenniDistr) {
		error("Empty path to OpenNI2 directory")
	}

	LIBS += -L$$OpenniDistr/Redist/
	win32: LIBS += -L$$OpenniDistr/Lib/

	INCLUDEPATH += = $$OpenniDistr/Include/
	LIBS += -lOpenNI2

	DEFINES += WITH_OPENNI2
}

!isEmpty(WITH_REALSENSE){

	win32 {
		RealSenseDistr =
		isEmpty(RealSenseDistr) {
			error("Empty path to RealSense directory")
		}

		contains(QMAKE_TARGET.arch, x86_64) {
			LIBS += -L$$RealSenseDistr/lib/x64
			LIBS += -L$$RealSenseDistr/bin/x64
		} else {
			LIBS += -L$$RealSenseDistr/lib/x86
			LIBS += -L$$RealSenseDistr/bin/x86
		}

		INCLUDEPATH += $$RealSenseDistr/include/
	}

	LIBS += -lrealsense2

	DEFINES += WITH_REALSENSE
}

INCLUDEPATH += $${FACE_SDK_PATH}/include

unix: LIBS += -ldl
win32: DEFINES += _USE_MATH_DEFINES

HEADERS += \
	mainwindow.h \
	viewwindow.h \
	drawfunction.h \
	facesdkparameters.h \
	videoframe.h \
	worker.h \
	database.h \
	depthsensorcapture.h

!isEmpty(WITH_OPENNI2){
	HEADERS += OpenniSource.h
}
else {
	HEADERS += RealSenseSource.h
}

SOURCES = \
	main.cpp \
	mainwindow.cpp \
	viewwindow.cpp \
	drawfunction.cpp \
	worker.cpp \
	database.cpp \
	depthsensorcapture.cpp

FORMS += \
	mainwindow.ui \
	viewwindow.ui
