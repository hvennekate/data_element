SOURCES += main.cpp \
    specdataitem.cpp \
    specmodel.cpp \
    specdescriptordelegate.cpp \
    specmodelitem.cpp \
    specplotwidget.cpp \
    utility-functions.cpp \
    specappwindow.cpp \
    specdelegate.cpp \
    specview.cpp \
    specdatapoint.cpp \
    specdescriptor.cpp \
    specdocktitle.cpp \
    canvaspicker.cpp \
    speczoomer.cpp \
    specplot.cpp \
    speclabeldialog.cpp \
    specrange.cpp \
    specminmaxvalidator.cpp \
    speckineticwidget.cpp \
    speckinetic.cpp \
    specfolderitem.cpp \
    speclogentryitem.cpp \
    speccanvasitem.cpp \
    exportdialog.cpp \
    exportlayoutitem.cpp \
    exportformatitem.cpp \
    speclogmessage.cpp \
    specdataview.cpp \
    speckineticview.cpp \
    speckineticmodel.cpp \
    speckineticrange.cpp \
    speckrcontextmenu.cpp \
    cutbyintensitydialog.cpp
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = ../bin/data_element
HEADERS += specdataitem.h \
    specmodel.h \
    specdescriptordelegate.h \
    specmodelitem.h \
    specplotwidget.h \
    utility-functions.h \
    specappwindow.h \
    specdelegate.h \
    specview.h \
    specdatapoint.h \
    specdescriptor.h \
    specdocktitle.h \
    canvaspicker.h \
    names.h \
    speczoomer.h \
    specplot.h \
    speclabeldialog.h \
    specrange.h \
    specminmaxvalidator.h \
    speckineticwidget.h \
    speckinetic.h \
    specfolderitem.h \
    speclogentryitem.h \
    speccanvasitem.h \
    exportdialog.h \
    exportlayoutitem.h \
    exportformatitem.h \
    speclogmessage.h \
    specdataview.h \
    speckineticview.h \
    speckineticmodel.h \
    speckineticrange.h \
    speckrcontextmenu.h \
    cutbyintensitydialog.h
unix {
INCLUDEPATH += . \
    /usr/include/qwt
LIBS += -lqwt \
    -lcln \
    -lginac
}
win32 {
INCLUDEPATH += . \
    C:/Qwt-5.2.1/include
LIBS += -LC:/Qwt-5.2.1/lib \
    -lqwt
}

RESOURCES += icons/application.qrc
