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
    ellipseitem.cpp \
    equationdialog.cpp \
    exportgraphicsdialog.cpp \
    filltab.cpp \
    generaltab.cpp \
    graphicsfactory.cpp \
    gridlayouthelper.cpp \
    gridtab.cpp \
    labelitem.cpp \
    labelrenderer.cpp \
    layoutboxitem.cpp \
    layouttab.cpp \
    lineitem.cpp \
    mainwindow.cpp \
    matrixmodel.cpp \
    memorywidget.cpp \
    pictureitem.cpp \
    plotitem.cpp \
    plotitemmanager.cpp \
    plotrenderitem.cpp \
    qgetoptions.cpp \
    selectionrect.cpp \
    sessionmodel.cpp \
    stroketab.cpp \
    svgitem.cpp \
    tabwidget.cpp \
    vectordialog.cpp \
    vectoreditordialog.cpp \
    vectorcurverenderitem.cpp \
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
    equationdialog.h \
    exportgraphicsdialog.h \
    filltab.h \
    generaltab.h \
    graphicsfactory.h \
    gridlayouthelper.h \
    gridtab.h \
    labelitem.h \
    labelrenderer.h \
    layoutboxitem.h \
    layouttab.h \
    lineitem.h \
    mainwindow.h \
    matrixmodel.h \
    memorywidget.h \
    pictureitem.h \
    plotitem.h \
    plotitemmanager.h \
    plotrenderitem.h \
    qgetoptions.h \
    selectionrect.h \
    sessionmodel.h \
    svgitem.h \
    stroketab.h \
    tabwidget.h \
    vectordialog.h \
    vectoreditordialog.h \
    vectormodel.h \
    vectorcurverenderitem.h \
    vectortablemodel.h \
    viewcommand.h \
    view.h \
    viewgridlayout.h \
    viewitemdialog.h \
    viewitem.h \
    viewmanager.h

FORMS += \
    aboutdialog.ui \
    curvetab.ui \
    datamanager.ui \
    debugdialog.ui \
    dialog.ui \
    editmultiplewidget.ui \
    equationtab.ui \
    exportgraphicsdialog.ui \
    filltab.ui \
    generaltab.ui \
    gridtab.ui \
    layouttab.ui \
    stroketab.ui \
    vectoreditordialog.ui \
    vectortab.ui \
    viewmanager.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
