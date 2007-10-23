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
    boxitem.cpp \
    builtingraphics.cpp \
    cartesianrenderitem.cpp \
    csddialog.cpp \
    curvedialog.cpp \
    databutton.cpp \
    databuttonaction.cpp \
    datadialog.cpp \
    datagui.cpp \
    datamanager.cpp \
    datasourcedialog.cpp \
    datatab.cpp \
    debugdialog.cpp \
    debugnotifier.cpp \
    dialog.cpp \
    dialoglaunchergui.cpp \
    dialogpage.cpp \
    dialogtab.cpp \
    document.cpp \
    editmultiplewidget.cpp \
    emailthread.cpp \
    ellipseitem.cpp \
    equationdialog.cpp \
    eventmonitordialog.cpp \
    eventmonitorentry.cpp \
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
    matrixmodel.cpp \
    matrixdialog.cpp \
    memorywidget.cpp \
    pictureitem.cpp \
    plotaxisitem.cpp \
    plotitem.cpp \
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
    boxitem.h \
    builtingraphics.h \
    cartesianrenderitem.h \
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
    debugdialog.h \
    debugnotifier.h \
    dialog.h \
    dialoglaunchergui.h \
    dialogpage.h \
    dialogtab.h \
    document.h \
    editmultiplewidget.h \
    ellipseitem.h \
    emailthread.h \
    equationdialog.h \
    eventmonitordialog.h \
    eventmonitorentry.h \
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
    matrixmodel.h \
    memorywidget.h \
    pictureitem.h \
    plotaxisitem.h \
    plotitem.h \
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
    csdtab.ui \
    curvetab.ui \
    datamanager.ui \
    debugdialog.ui \
    dialog.ui \
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
    matrixtab.ui \
    powerspectrumtab.ui \
    scalareditordialog.ui \
    scalartab.ui \
    stroketab.ui \
    vectoreditordialog.ui \
    vectortab.ui \
    viewmanager.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
