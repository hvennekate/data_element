include(prefix.pri)

SOURCES += main.cpp \
    cutbyintensitydialog.cpp \
    specappwindow.cpp \
    specplotwidget.cpp \
    utility-functions.cpp \
    kinetic/speckineticwidget.cpp \
    log/speclogentryitem.cpp \
    log/speclogmessage.cpp \
    model/exportdialog.cpp \
    model/exportlayoutitem.cpp \
    model/exportformatitem.cpp \
    model/specdelegate.cpp \
    model/specview.cpp \
    model/specdescriptor.cpp \
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
    actionlib/specremovedataaction.cpp \
    actionlib/specaveragedataaction.cpp \
    actionlib/specexchangedatacommand.cpp \
    model/specsvgitem.cpp \
    actionlib/specaddsvgitem.cpp \
    actionlib/specprintplotaction.cpp \
    actionlib/specresizesvgcommand.cpp \
    kinetic/specmetaitem.cpp \
    kinetic/specmetamodel.cpp \
    kinetic/specmetaview.cpp \
    actionlib/specnewmetaitemaction.cpp \
    actionlib/specmanageconnectionscommand.cpp \
    actionlib/specaddconnectionscommand.cpp \
    actionlib/specdeleteconnectionscommand.cpp \
    actionlib/specaddconnectionsaction.cpp \
    model/specmimeconverter.cpp \
    log/speclogtodataconverter.cpp \
    kinetic/specmetaparser.cpp \
    kinetic/specmetavariable.cpp \
    kinetic/specintegralvariable.cpp \
    kinetic/specxvariable.cpp \
    kinetic/specdescriptorvariable.cpp \
    kinetic/specyvariable.cpp \
    kinetic/specminvariable.cpp \
    kinetic/specmaxvariable.cpp \
    kinetic/specmetadelegate.cpp \
    kinetic/specmetarange.cpp \
    log/speclogwidget.cpp \
    actionlib/speceditdescriptorcommand.cpp \
    plot/specplotstyle.cpp \
    specstreamable.cpp \
    actionlib/specmetarangecommand.cpp \
    model/specgenericmimeconverter.cpp \
    model/spectextmimeconverter.cpp \
    spectral/dataitemproperties.cpp \
    kinetic/metaitemproperties.cpp \
    model/svgitemproperties.cpp \
    model/specsvgunitbutton.cpp \
    specsplitter.cpp \
    actionlib/specplotlabelcommand.cpp \
    actionlib/speclabelaction.cpp

HEADERS += cutbyintensitydialog.h \
    specappwindow.h \
    specplotwidget.h \
    names.h \
    utility-functions.h \
    kinetic/speckineticwidget.h \
    log/speclogentryitem.h \
    log/speclogmessage.h \
    model/exportdialog.h \
    model/exportlayoutitem.h \
    model/exportformatitem.h \
    model/specdelegate.h \
    model/specview.h \
    model/specdescriptor.h \
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
    actionlib/specremovedataaction.h \
    actionlib/specaveragedataaction.h \
    actionlib/specexchangedatacommand.h \
    model/specsvgitem.h \
    actionlib/specaddsvgitem.h \
    actionlib/specprintplotaction.h \
    actionlib/specresizesvgcommand.h \
    kinetic/specmetaitem.h \
    kinetic/specmetamodel.h \
    kinetic/specmetaview.h \
    actionlib/specnewmetaitemaction.h \
    actionlib/specmanageconnectionscommand.h \
    actionlib/specaddconnectionscommand.h \
    actionlib/specdeleteconnectionscommand.h \
    actionlib/specaddconnectionsaction.h \
    model/specmimeconverter.h \
    log/speclogtodataconverter.h \
    kinetic/specmetaparser.h \
    kinetic/specmetavariable.h \
    kinetic/specintegralvariable.h \
    kinetic/specxvariable.h \
    kinetic/specdescriptorvariable.h \
    kinetic/specyvariable.h \
    kinetic/specminvariable.h \
    kinetic/specmaxvariable.h \
    kinetic/specmetadelegate.h \
    kinetic/specmetarange.h \
    log/speclogwidget.h \
    actionlib/speceditdescriptorcommand.h \
    plot/specplotstyle.h \
    specstreamable.h \
    actionlib/specmetarangecommand.h \
    model/specgenericmimeconverter.h \
    model/spectextmimeconverter.h \
    spectral/dataitemproperties.h \
    kinetic/metaitemproperties.h \
    model/svgitemproperties.h \
    model/specsvgunitbutton.h \
    specsplitter.h \
    actionlib/specplotlabelcommand.h \
    actionlib/speclabelaction.h

INCLUDEPATH += kinetic \
	log \
	model \
	plot \
	spectral \
	actionlib


TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt

unix {
INCLUDEPATH += . \
    /usr/include/qwt
LIBS += -lqwt \
	-lmuparser
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

QMAKE_CXXFLAGS += -std=c++0x

FORMS += \
    spectral/dataitemproperties.ui \
    kinetic/metaitemproperties.ui \
    model/svgitemproperties.ui
