TEMPLATE = app
LANGUAGE = C++
TARGET   = NifSkope

QT += xml opengl network

CONFIG += qt release thread warn_on

#CONFIG += fsengine

# uncomment this if you want all the messages to be logged to stdout
#CONFIG += console

# uncomment this if you want the text stats gl option
#DEFINES += USE_GL_QPAINTER

DESTDIR = ./

HEADERS += \
	niftypes.h \
	nifvalue.h \
	basemodel.h \
	kfmmodel.h \
	nifmodel.h \
	glview.h \
	message.h \
	nifproxy.h \
	nifskope.h \
	spellbook.h \
	gl/*.h \
	widgets/*.h \
	spells/*.h

SOURCES += \
	niftypes.cpp \
	nifvalue.cpp \
	basemodel.cpp \
	kfmmodel.cpp \
	kfmxml.cpp \
	nifmodel.cpp \
	nifxml.cpp \
	glview.cpp \
	message.cpp \
	nifdelegate.cpp \
	nifproxy.cpp \
	nifskope.cpp \
	spellbook.cpp \
	gl/*.cpp \
	widgets/*.cpp \
	spells/*.cpp \
	NvTriStrip/*.cpp

RESOURCES += nifskope.qrc

fsengine {
	DEFINES += FSENGINE
	HEADERS += fsengine/*.h
	SOURCES += fsengine/*.cpp
}

win32 {
    RC_FILE = icon.rc
	DEFINES += EDIT_ON_ACTIVATE
}

win32:console {
	LIBS += -lmingw32 -lqtmain
}

console {
	DEFINES += NO_MESSAGEHANDLER
}
