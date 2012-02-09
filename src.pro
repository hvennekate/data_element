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
    actionlib/specimportspecaction.cpp \
    model/specviewstate.cpp \
    log/speclogmodel.cpp \
    log/speclogview.cpp \
    actionlib/speccopyaction.cpp \
    actionlib/specpasteaction.cpp \
    actionlib/speccutaction.cpp \
    plot/specspectrumplot.cpp \
    actionlib/specplotmovecommand.cpp \
	textEditor/specsimpletextedit.cpp \
    actionlib/changeplotstyleaction.cpp \
    actionlib/specstylecommand.cpp \
    actionlib/spectreeaction.cpp \
    actionlib/specmulticommand.cpp \
    actionlib/specmergeaction.cpp \
    actionlib/specremovedatacommand.cpp \
    actionlib/specmanagedatacommand.cpp \
    actionlib/specinsertdatacommand.cpp \
    actionlib/specremovedataaction.cpp \
    actionlib/specaveragedataaction.cpp \
    actionlib/specexchangedatacommand.cpp \
    model/specsvgitem.cpp \
    actionlib/specaddsvgitem.cpp \
    actionlib/specprintplotaction.cpp \
    actionlib/specresizesvgcommand.cpp \
    kinetic/specmetafilter.cpp \
    kinetic/specmetaitem.cpp \
    kinetic/specdescriptordescriptorfilter.cpp \
    kinetic/specmetamodel.cpp \
    kinetic/specmetaview.cpp \
    actionlib/specnewmetaitemaction.cpp \
    actionlib/specmanageconnectionscommand.cpp \
    actionlib/specaddconnectionscommand.cpp \
    actionlib/specdeleteconnectionscommand.cpp \
    actionlib/specaddconnectionsaction.cpp \
    kinetic/descriptorxrangefilter.cpp

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
    actionlib/specimportspecaction.h \
    model/specviewstate.h \
    log/speclogmodel.h \
    log/speclogview.h \
    actionlib/speccopyaction.h \
    actionlib/specpasteaction.h \
    actionlib/speccutaction.h \
    plot/specspectrumplot.h \
    actionlib/specplotmovecommand.h \
	textEditor/specsimpletextedit.h \
    actionlib/changeplotstyleaction.h \
    actionlib/specstylecommand.h \
    actionlib/spectreeaction.h \
    actionlib/specmulticommand.h \
    actionlib/specmergeaction.h \
    actionlib/specremovedatacommand.h \
    actionlib/specmanagedatacommand.h \
    actionlib/specinsertdatacommand.h \
    actionlib/specremovedataaction.h \
    actionlib/specaveragedataaction.h \
    actionlib/specexchangedatacommand.h \
    model/specsvgitem.h \
    actionlib/specaddsvgitem.h \
    actionlib/specprintplotaction.h \
    actionlib/specresizesvgcommand.h \
    kinetic/specmetafilter.h \
    kinetic/specmetaitem.h \
    kinetic/specdescriptordescriptorfilter.h \
    kinetic/specmetamodel.h \
    kinetic/specmetaview.h \
    actionlib/specnewmetaitemaction.h \
    actionlib/specmanageconnectionscommand.h \
    actionlib/specaddconnectionscommand.h \
    actionlib/specdeleteconnectionscommand.h \
    actionlib/specaddconnectionsaction.h \
    kinetic/descriptorxrangefilter.h

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

QT += svg
