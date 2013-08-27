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
    tst_moveplotcommand.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../ \
	../kinetic \
	../log \
	../model \
	../plot \
	../spectral


unix {
INCLUDEPATH += . \
    /usr/include/qwt
LIBS += -lqwt \
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
    tst_moveplotcommand.h

DEFINES += DOUBLEDEVIATIONCORRECTION=1
DEFINES += NUMBEROFFRACTIONBITSINDOUBLE=52
