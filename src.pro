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
    actionlib/commands/specundocommand.cpp \
    actionlib/actions/specundoaction.cpp \
    actionlib/actions/specdeleteaction.cpp \
    actionlib/commands/specdeletecommand.cpp \
    actionlib/actions/specaddfolderaction.cpp \
    actionlib/commands/specaddfoldercommand.cpp \
    model/specgenealogy.cpp \
    actionlib/commands/specmanageitemscommand.cpp \
    actionlib/commands/specmovecommand.cpp \
    actionlib/actions/specimportspecaction.cpp \
    model/specviewstate.cpp \
    log/speclogmodel.cpp \
    log/speclogview.cpp \
    actionlib/actions/speccopyaction.cpp \
    actionlib/actions/specpasteaction.cpp \
    actionlib/actions/speccutaction.cpp \
    plot/specspectrumplot.cpp \
    textEditor/specsimpletextedit.cpp \
    actionlib/actions/changeplotstyleaction.cpp \
    actionlib/commands/specstylecommand.cpp \
    actionlib/actions/spectreeaction.cpp \
    actionlib/commands/specmulticommand.cpp \
    actionlib/actions/specmergeaction.cpp \
    actionlib/actions/specremovedataaction.cpp \
    actionlib/actions/specaveragedataaction.cpp \
    actionlib/commands/specexchangedatacommand.cpp \
    model/specsvgitem.cpp \
    actionlib/actions/specaddsvgitem.cpp \
    actionlib/actions/specprintplotaction.cpp \
    actionlib/commands/specresizesvgcommand.cpp \
    kinetic/specmetaitem.cpp \
    kinetic/specmetamodel.cpp \
    kinetic/specmetaview.cpp \
    actionlib/actions/specnewmetaitemaction.cpp \
    actionlib/commands/specmanageconnectionscommand.cpp \
    actionlib/commands/specaddconnectionscommand.cpp \
    actionlib/commands/specdeleteconnectionscommand.cpp \
    actionlib/actions/specaddconnectionsaction.cpp \
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
    actionlib/commands/speceditdescriptorcommand.cpp \
    plot/specplotstyle.cpp \
    specstreamable.cpp \
    actionlib/commands/specmetarangecommand.cpp \
    model/specgenericmimeconverter.cpp \
    model/spectextmimeconverter.cpp \
    spectral/dataitemproperties.cpp \
    kinetic/metaitemproperties.cpp \
    model/svgitemproperties.cpp \
    model/specsvgunitbutton.cpp \
    specsplitter.cpp \
    actionlib/commands/specplotlabelcommand.cpp \
    actionlib/actions/speclabelaction.cpp \
    specprofiler.cpp \
    actionlib/actions/genericexportaction.cpp \
    actionlib/actions/specitemaction.cpp \
    actionlib/actions/specrequiresitemaction.cpp \
    model/specmimetextexporter.cpp \
    lmfit/lmmin.c \
    kinetic/specfitcurve.cpp \
    actionlib/commands/specexchangefitcurvecommand.cpp \
    actionlib/actions/specitempropertiesaction.cpp \
    actionlib/actions/specaddfitaction.cpp \
    actionlib/actions/specconductfitaction.cpp \
    actionlib/actions/specremovefitaction.cpp \
    actionlib/commands/spectogglefitstylecommand.cpp \
    actionlib/actions/spectogglefitstyleaction.cpp \
    actionlib/speccommandgenerator.cpp \
    actionlib/actions/specselectconnectedaction.cpp \
    actionlib/specworkerthread.cpp \
    actionlib/actions/specsetmultilineaction.cpp \
    actionlib/commands/specdeletedescriptorcommand.cpp \
    actionlib/commands/specrenamedescriptorcommand.cpp \
    actionlib/actions/specdescriptoreditaction.cpp \
    utilities/bzipiodevice.cpp \
    model/specmimefileimporter.cpp \
    asciiexporter.cpp \
    kinetic/specminposvariable.cpp \
    kinetic/specmaxposvariable.cpp \
    actionlib/actions/spectiltmatrixaction.cpp \
    actionlib/actions/specexchangedescriptorxdialog.cpp \
    spectral/specpefile.cpp \
    actionlib/actions/specspectrumcalculatoraction.cpp \
    actionlib/actions/specformulavalidator.cpp \
    actionlib/actions/spectrumcalculatordialog.cpp \
    specdockwidget.cpp \
    actionlib/actions/specflattentreeaction.cpp \
    actionlib/actions/specnormalizeaction.cpp \
    actionlib/actions/specmergedialog.cpp \
    actionlib/actions/specconnectionsaction.cpp \
    model/textedit.cpp \
    actionlib/commands/specsingleitemcommand.cpp \
    actionlib/commands/specmultipleitemcommand.cpp \
    specshortcutdialog.cpp \
    actionlib/commands/specexchangefiltercommand.cpp \
    model/specdatapointfilter.cpp \
    actionlib/actions/specfitaction.cpp \
    plot/specfiltergenerator.cpp \
    model/specdescriptorcomparisoncriterion.cpp \
    model/specdatatokineticmime.cpp \
    model/specmatrixmimeexporter.cpp \
    actionlib/actions/matrixexportaction.cpp \
    plot/spec3dquickplot.cpp \
    actionlib/actions/toggle3dplotaction.cpp \
    log/editimportdirsaction.cpp \
    actionlib/commands/specmultilinecommand.cpp \
    kinetic/editorWidgets/metavariableeditor.cpp \
    kinetic/editorWidgets/exclusivevalidator.cpp \
    kinetic/editorWidgets/widgetlist.cpp \
    kinetic/editorWidgets/fitvariableeditor.cpp \
    kinetic/editorWidgets/abstractvariableeditor.cpp \
    kinetic/editorWidgets/activefitvareditor.cpp

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
    actionlib/commands/specundocommand.h \
    actionlib/actions/specundoaction.h \
    actionlib/actions/specdeleteaction.h \
    actionlib/commands/specdeletecommand.h \
    actionlib/actions/specaddfolderaction.h \
    actionlib/commands/specaddfoldercommand.h \
    model/specgenealogy.h \
    actionlib/commands/specmanageitemscommand.h \
    actionlib/commands/specmovecommand.h \
    actionlib/actions/specimportspecaction.h \
    model/specviewstate.h \
    log/speclogmodel.h \
    log/speclogview.h \
    actionlib/actions/speccopyaction.h \
    actionlib/actions/specpasteaction.h \
    actionlib/actions/speccutaction.h \
    plot/specspectrumplot.h \
    textEditor/specsimpletextedit.h \
    actionlib/actions/changeplotstyleaction.h \
    actionlib/commands/specstylecommand.h \
    actionlib/actions/spectreeaction.h \
    actionlib/commands/specmulticommand.h \
    actionlib/actions/specmergeaction.h \
    actionlib/actions/specremovedataaction.h \
    actionlib/actions/specaveragedataaction.h \
    actionlib/commands/specexchangedatacommand.h \
    model/specsvgitem.h \
    actionlib/actions/specaddsvgitem.h \
    actionlib/actions/specprintplotaction.h \
    actionlib/commands/specresizesvgcommand.h \
    kinetic/specmetaitem.h \
    kinetic/specmetamodel.h \
    kinetic/specmetaview.h \
    actionlib/actions/specnewmetaitemaction.h \
    actionlib/commands/specmanageconnectionscommand.h \
    actionlib/commands/specaddconnectionscommand.h \
    actionlib/commands/specdeleteconnectionscommand.h \
    actionlib/actions/specaddconnectionsaction.h \
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
    actionlib/commands/speceditdescriptorcommand.h \
    plot/specplotstyle.h \
    specstreamable.h \
    actionlib/commands/specmetarangecommand.h \
    model/specgenericmimeconverter.h \
    model/spectextmimeconverter.h \
    spectral/dataitemproperties.h \
    kinetic/metaitemproperties.h \
    model/svgitemproperties.h \
    model/specsvgunitbutton.h \
    specsplitter.h \
    actionlib/commands/specplotlabelcommand.h \
    actionlib/actions/speclabelaction.h \
    specprofiler.h \
    actionlib/actions/genericexportaction.h \
    actionlib/actions/specitemaction.h \
    actionlib/actions/specrequiresitemaction.h \
    model/specmimetextexporter.h \
    lmfit/lmmin.h \
    kinetic/specfitcurve.h \
    actionlib/commands/specexchangefitcurvecommand.h \
    actionlib/actions/specitempropertiesaction.h \
    actionlib/actions/specaddfitaction.h \
    actionlib/actions/specconductfitaction.h \
    actionlib/actions/specremovefitaction.h \
    actionlib/commands/spectogglefitstylecommand.h \
    actionlib/actions/spectogglefitstyleaction.h \
    actionlib/speccommandgenerator.h \
    actionlib/actions/specselectconnectedaction.h \
    actionlib/specworkerthread.h \
    actionlib/actions/specsetmultilineaction.h \
    actionlib/commands/specdeletedescriptorcommand.h \
    actionlib/commands/specrenamedescriptorcommand.h \
    actionlib/actions/specdescriptoreditaction.h \
    utilities/bzipiodevice.h \
    model/specmimefileimporter.h \
    asciiexporter.h \
    kinetic/specminposvariable.h \
    kinetic/specmaxposvariable.h \
    actionlib/actions/spectiltmatrixaction.h \
    actionlib/actions/specexchangedescriptorxdialog.h \
    spectral/specpefile.h \
    actionlib/actions/specspectrumcalculatoraction.h \
    actionlib/actions/specformulavalidator.h \
    actionlib/actions/spectrumcalculatordialog.h \
    specdockwidget.h \
    actionlib/actions/specflattentreeaction.h \
    actionlib/actions/specnormalizeaction.h \
    actionlib/actions/specmergedialog.h \
    actionlib/actions/specconnectionsaction.h \
    model/textedit.h \
    actionlib/commands/specsingleitemcommand.h \
    actionlib/commands/specmultipleitemcommand.h \
    specshortcutdialog.h \
    actionlib/commands/specexchangefiltercommand.h \
    model/specdatapointfilter.h \
    actionlib/actions/specfitaction.h \
    plot/specfiltergenerator.h \
    model/specdescriptorcomparisoncriterion.h \
    model/specdatatokineticmime.h \
    model/specmatrixmimeexporter.h \
    actionlib/actions/matrixexportaction.h \
    plot/spec3dquickplot.h \
    actionlib/actions/toggle3dplotaction.h \
    log/editimportdirsaction.h \
    actionlib/commands/specmultilinecommand.h \
    kinetic/editorWidgets/metavariableeditor.h \
    kinetic/editorWidgets/exclusivevalidator.h \
    kinetic/editorWidgets/widgetlist.h \
    kinetic/editorWidgets/fitvariableeditor.h \
    kinetic/editorWidgets/abstractvariableeditor.h \
    kinetic/editorWidgets/activefitvareditor.h

INCLUDEPATH += kinetic \
	kinetic/editorWidgets \
	log \
	model \
	plot \
	spectral \
	actionlib \
	actionlib/actions \
	actionlib/commands \
	lmfit \
	utilities


TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    opengl

unix {
INCLUDEPATH += . \
	/home/hendrik/Programme/qwt/include \
	/home/hendrik/Downloads/qwtplot3d/include
LIBS += -L/home/hendrik/Programme/qwt/lib \
	-L/home/hendrik/Downloads/qwtplot3d/lib \
	-lqwtplot3d \
	-lqwt \
	-lmuparser \
	-lbz2 \
	-lgsl \
	-lgslcblas \
	-lGLU
}
win32 {
WINPREFIX = Z:/home/hendrik/.wine/drive_c
INCLUDEPATH += . \
    $$WINPREFIX/qwt/include \
    $$WINPREFIX/gsl/include \
    $$WINPREFIX/bzip2/include \
    $$WINPREFIX/muparser/include \
    $$WINPREFIX/qwtplot3d/include
LIBS += -L$$WINPREFIX/qwt/lib \
	-L$$WINPREFIX/gsl/bin \
	-L$$WINPREFIX/bzip2/lib \
	-L$$WINPREFIX/qwtplot3d/lib \
	-lqwt \
	-llibgslcblas-0 \
	-llibgsl-0 \
	-lbzip2 \
	-lqwtplot3d \
	$$WINPREFIX/muparser/lib/libmuparser.a
# 	-L$$WINPREFIX/muparser/lib \
# 	-lmuparser
# LIBS += $$WININCLUDES/qt5/lib/libqwt.a \
#     $$WINLIBS/libmuparser.a \
#     $$WINLIBS/libgslcblas.a \
#     $$WINLIBS/libgsl.a \
#     /home/hendrik/.wine/drive_c/bzip2/release/libbzip2.a
DEFINES += WIN32BUILD \
    QT_DLL \
    QWT_DLL \
    QWT3D_DLL
#DEPENDPATH += C:/Users/Hendrik/Downloads/muparser_v2_2_2/lib
RESOURCES += oxygen.qrc
}
CONFIG += qwt

TARGET = data_element

RESOURCES += icons/application.qrc \
    icons/fromOxygen.qrc

QT += svg printsupport opengl

#QMAKE_CXXFLAGS += -std=c++0x

FORMS += \
    spectral/dataitemproperties.ui \
    kinetic/metaitemproperties.ui \
    model/svgitemproperties.ui \
    actionlib/actions/specexchangedescriptorxdialog.ui \
    actionlib/actions/spectrumcalculatordialog.ui \
    actionlib/actions/specnormalizeactiondialog.ui \
    actionlib/actions/specmergedialog.ui \
    actionlib/actions/averagedialog.ui \
    specshortcutdialog.ui \
    model/matrixexportdialog.ui \
    logconverterimportdirectoriesdialog.ui \
    kinetic/editorWidgets/metavariableeditor.ui \
    kinetic/editorWidgets/widgetlist.ui \
    kinetic/editorWidgets/fitvariableeditor.ui

OTHER_FILES += icons/oxygen/index.theme \
    icons/oxygen/geticons.py

CODECFORTR = UTF-8

DEFINES += DOUBLEDEVIATIONCORRECTION=1
DEFINES += NUMBEROFFRACTIONBITSINDOUBLE=52
#DEFINES += DEBUGCOMMANDREADER
