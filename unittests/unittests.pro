#-------------------------------------------------
#
# Project created by QtCreator 2012-06-20T14:18:24
#
#-------------------------------------------------

QT       += svg testlib

TARGET = tst_specmetavariable
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    tst_specmetavariable.cpp \
    tst_specmetaparser.cpp \
    tst_main.cpp \
    tst_moveplotcommand.cpp \
    tst_shortcutactivation.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../ \
	../kinetic \
	../log \
	../model \
	../plot \
	../spectral


unix {
INCLUDEPATH += . \
    /home/hendrik/Programme/qwt-6.1.0/include
LIBS += -L/home/hendrik/Programme/qwt-6.1.0/lib \
	-lqwt \
	-lmuparser \
	-lbz2 \
	-lgsl \
	-lgslcblas
}

CONFIG += qwt

OBJECTS += ../../build/objects/[^m]*.o \
	../../build/objects/moc*.o \
	../../build/objects/meta*.o

HEADERS += \
    tst_specmetavariable.h \
    tst_specmetaparser.h \
    tst_moveplotcommand.h \
    tst_shortcutactivation.h

DEFINES += DOUBLEDEVIATIONCORRECTION=1
DEFINES += NUMBEROFFRACTIONBITSINDOUBLE=52

OBJECTS_DIR = ../../tests/objects
MOC_DIR = ../../tests/moc
DESTDIR = ../../tests/target
DLLDESTDIR = ../../tests/target
UI_DIR = ../../tests/uic
UI_HEADERS_DIR = ../../tests/uic-headers
UI_SOURCES_DIR = ../../tests/uic-src
LIBS += -L../../tests/target
RCC_DIR = ../../tests/resources
