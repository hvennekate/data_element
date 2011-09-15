include(prefix.pri)

SOURCES += main.cpp \
    cutbyintensitydialog.cpp \
    specappwindow.cpp \
    specdocktitle.cpp \
    speckrcontextmenu.cpp \
    speclabeldialog.cpp \
    specminmaxvalidator.cpp \
    specplotwidget.cpp \
    utility-functions.cpp \
    kinetic/speckinetic.cpp \
    kinetic/speckineticwidget.cpp \
    kinetic/speckineticview.cpp \
    kinetic/speckineticmodel.cpp \
    kinetic/speckineticrange.cpp \
    log/speclogentryitem.cpp \
    log/speclogmessage.cpp \
    model/exportdialog.cpp \
    model/exportlayoutitem.cpp \
    model/exportformatitem.cpp \
    model/specdelegate.cpp \
    model/specview.cpp \
    model/specdescriptor.cpp \
    model/specdescriptordelegate.cpp \
    model/specmodel.cpp \
    model/specmodelitem.cpp \
    model/specfolderitem.cpp \
    plot/canvaspicker.cpp \
    plot/speczoomer.cpp \
    plot/specplot.cpp \
    plot/specrange.cpp \
    plot/speccanvasitem.cpp \
    spectral/specdataitem.cpp \
    spectral/specdatapoint.cpp \
    spectral/specdataview.cpp \
    actionlib/specactionlibrary.cpp \
    actionlib/specundocommand.cpp \
    actionlib/specundoaction.cpp \
    actionlib/specdeleteaction.cpp \
    actionlib/specdeletecommand.cpp \
    actionlib/specaddfolderaction.cpp \
    actionlib/specaddfoldercommand.cpp \
    model/specgenealogy.cpp \
    actionlib/specmanageitemscommand.cpp \
    actionlib/specmovecommand.cpp \
    actionlib/specimportspecaction.cpp

HEADERS += cutbyintensitydialog.h \
    specappwindow.h \
    specdocktitle.h \
    speckrcontextmenu.h \
    speclabeldialog.h \
    specminmaxvalidator.h \
    specplotwidget.h \
    names.h \
    utility-functions.h \
    kinetic/speckinetic.h \
    kinetic/speckineticwidget.h \
    kinetic/speckineticview.h \
    kinetic/speckineticmodel.h \
    kinetic/speckineticrange.h \
    log/speclogentryitem.h \
    log/speclogmessage.h \
    model/exportdialog.h \
    model/exportlayoutitem.h \
    model/exportformatitem.h \
    model/specdelegate.h \
    model/specview.h \
    model/specdescriptor.h \
    model/specdescriptordelegate.h \
    model/specmodel.h \
    model/specmodelitem.h \
    model/specfolderitem.h \
    plot/canvaspicker.h \
    plot/speczoomer.h \
    plot/specplot.h \
    plot/specrange.h \
    plot/speccanvasitem.h \
    spectral/specdataitem.h \
    spectral/specdatapoint.h \
    spectral/specdataview.h \
    actionlib/specactionlibrary.h \
    actionlib/specundocommand.h \
    actionlib/specundoaction.h \
    actionlib/specdeleteaction.h \
    actionlib/specdeletecommand.h \
    actionlib/specaddfolderaction.h \
    actionlib/specaddfoldercommand.h \
    model/specgenealogy.h \
    actionlib/specmanageitemscommand.h \
    actionlib/specmovecommand.h \
    actionlib/specimportspecaction.h

INCLUDEPATH += kinetic \
	log \
	model \
	plot \
	spectral


TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt

unix {
INCLUDEPATH += . \
    /usr/include/qwt
LIBS += -lqwt \
    -lcln \
    -lginac
}
#win32 {
#INCLUDEPATH += . \
#    C:/Qwt-5.2.1/include
#LIBS += -LC:/Qwt-5.2.1/lib \
#    -lqwt
#}
CONFIG += qwt

TARGET = data_element

RESOURCES += icons/application.qrc
