include($$PWD/../../kst.pri)

QT += gui network svg xml opengl qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstapp
DESTDIR = $$OUTPUT_DIR/lib
win32:CONFIG += staticlib

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$OUTPUT_DIR/src/widgets \
    $$OUTPUT_DIR/src/libkstapp/tmp

LIBS += -lkst -lkstmath -lkstwidgets

SOURCES += \
    application.cpp \
    applicationsettings.cpp \
    applicationsettingsdialog.cpp \
    axis.cpp \
    basicplugindialog.cpp \
    boxitem.cpp \
    builtingraphics.cpp \
    cartesianrenderitem.cpp \
    changedatasampledialog.cpp \
    changefiledialog.cpp \
    choosecolordialog.cpp \
    csddialog.cpp \
    curvedialog.cpp \
    databutton.cpp \
    databuttonaction.cpp \
    datadialog.cpp \
    datagui.cpp \
    datamanager.cpp \
    datasourcedialog.cpp \
    datatab.cpp \
    datawizard.cpp \
    debugdialog.cpp \
    debugnotifier.cpp \
    dialog.cpp \
    dialogdefaults.cpp \
    dialoglaunchergui.cpp \
    dialogpage.cpp \
    dialogtab.cpp \
    differentiatecurvesdialog.cpp \
    document.cpp \
    editmultiplewidget.cpp \
    ellipseitem.cpp \
    equationdialog.cpp \
    eventmonitordialog.cpp \
    exportgraphicsdialog.cpp \
    filltab.cpp \
    generaltab.cpp \
    graphicsfactory.cpp \
    gridlayouthelper.cpp \
    gridtab.cpp \
    histogramdialog.cpp \
    imagedialog.cpp \
    labelitem.cpp \
    labelrenderer.cpp \
    layoutboxitem.cpp \
    layouttab.cpp \
    lineitem.cpp \
    mainwindow.cpp \
    matrixdialog.cpp \
    matrixeditordialog.cpp \
    matrixmodel.cpp \
    memorywidget.cpp \
    pictureitem.cpp \
    plotaxisitem.cpp \
    plotitem.cpp \
    plotrenderitemdialog.cpp \
    plotitemmanager.cpp \
    plotrenderitem.cpp \
    powerspectrumdialog.cpp \
    qgetoptions.cpp \
    scalardialog.cpp \
    scalareditordialog.cpp \
    scalarmodel.cpp \
    scalartablemodel.cpp \
    scene.cpp \
    selectionrect.cpp \
    sessionmodel.cpp \
    stringmodel.cpp \
    stringtablemodel.cpp \
    stringeditordialog.cpp \
    stroketab.cpp \
    svgitem.cpp \
    tabwidget.cpp \
    vectordialog.cpp \
    vectoreditordialog.cpp \
    vectormodel.cpp \
    vectortablemodel.cpp \
    viewcommand.cpp \
    view.cpp \
    viewgridlayout.cpp \
    viewitem.cpp \
    viewitemdialog.cpp \
    viewmanager.cpp

HEADERS += \
    application.h \
    applicationsettings.h \
    applicationsettingsdialog.h \
    axis.h \
    basicplugindialog.h \
    boxitem.h \
    builtingraphics.h \
    cartesianrenderitem.h \
    changedatasampledialog.h \
    changefiledialog.h \
    choosecolordialog.h \
    csddialog.h \
    curvedialog.h \
    databutton.h \
    databuttonaction.h \
    datadialog.h \
    datagui.h \
    datamanager.h \
    dataref.h \
    datasourcedialog.h \
    datatab.h \
    datawizard.h \
    debugdialog.h \
    debugnotifier.h \
    dialog.h \
    dialogdefaults.h \
    dialoglaunchergui.h \
    dialogpage.h \
    dialogtab.h \
    differentiatecurvesdialog.h \
    document.h \
    editmultiplewidget.h \
    ellipseitem.h \
    equationdialog.h \
    eventmonitordialog.h \
    exportgraphicsdialog.h \
    filltab.h \
    generaltab.h \
    graphicsfactory.h \
    gridlayouthelper.h \
    gridtab.h \
    histogramdialog.h \
    imagedialog.h \
    labelitem.h \
    labelrenderer.h \
    layoutboxitem.h \
    layouttab.h \
    lineitem.h \
    mainwindow.h \
    matrixdialog.h \
    matrixeditordialog.h \
    matrixmodel.h \
    memorywidget.h \
    pictureitem.h \
    plotaxisitem.h \
    plotitem.h \
    plotrenderitemdialog.h \
    plotitemmanager.h \
    plotrenderitem.h \
    powerspectrumdialog.h \
    qgetoptions.h \
    scalardialog.h \
    scalareditordialog.h \
    scalarmodel.h \
    scalartablemodel.h \
    scene.h \
    selectionrect.h \
    sessionmodel.h \
    stringmodel.h \
    stringtablemodel.h \
    stringeditordialog.h \
    svgitem.h \
    stroketab.h \
    tabwidget.h \
    vectordialog.h \
    vectoreditordialog.h \
    vectormodel.h \
    vectortablemodel.h \
    viewcommand.h \
    view.h \
    viewgridlayout.h \
    viewitemdialog.h \
    viewitem.h \
    viewitemzorder.h \
    viewmanager.h

FORMS += \
    aboutdialog.ui \
    basicplugintab.ui \
    changedatasampledialog.ui \
    changefiledialog.ui \
    choosecolordialog.ui \
    csdtab.ui \
    curvetab.ui \
    datamanager.ui \
    datawizardpagedatapresentation.ui \
    datawizardpagedatasource.ui \
    datawizardpagefilters.ui \
    datawizardpageplot.ui \
    datawizardpagevectors.ui \
    debugdialog.ui \
    dialog.ui \
    differentiatecurvesdialog.ui \
    editmultiplewidget.ui \
    equationtab.ui \
    eventmonitortab.ui \
    exportgraphicsdialog.ui \
    filltab.ui \
    generaltab.ui \
    gridtab.ui \
    histogramtab.ui \
    imagetab.ui \
    layouttab.ui \
    matrixeditordialog.ui \
    matrixtab.ui \
    plotrenderitemdialog.ui \
    powerspectrumtab.ui \
    scalareditordialog.ui \
    scalartab.ui \
    stringeditordialog.ui \
    stroketab.ui \
    vectoreditordialog.ui \
    vectortab.ui \
    viewmanager.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
