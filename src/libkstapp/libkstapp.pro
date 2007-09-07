include($$PWD/../../kst.pri)

QT += gui network svg xml opengl qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstapp
DESTDIR = $$OUTPUT_DIR/lib

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
    axis.cpp \
    boxitem.cpp \
    builtingraphics.cpp \
    datamanager.cpp \
    debugdialog.cpp \
    debugnotifier.cpp \
    dialog.cpp \
    dialogpage.cpp \
    dialogtab.cpp \
    document.cpp \
    ellipseitem.cpp \
    exportgraphicsdialog.cpp \
    filltab.cpp \
    graphicsfactory.cpp \
    labelitem.cpp \
    labelrenderer.cpp \
    lineitem.cpp \
    mainwindow.cpp \
    matrixmodel.cpp \
    memorywidget.cpp \
    pictureitem.cpp \
    plotitem.cpp \
    plotrenderitem.cpp \
    qgetoptions.cpp \
    vectorcurverenderitem.cpp \
    sessionmodel.cpp \
    stroketab.cpp \
    svgitem.cpp \
    tabwidget.cpp \
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
    axis.h \
    boxitem.h \
    builtingraphics.h \
    datamanager.h \
    dataref.h \
    debugdialog.h \
    debugnotifier.h \
    dialog.h \
    dialogpage.h \
    dialogtab.h \
    document.h \
    ellipseitem.h \
    exportgraphicsdialog.h \
    filltab.h \
    graphicsfactory.h \
    labelitem.h \
    labelrenderer.h \
    lineitem.h \
    mainwindow.h \
    matrixmodel.h \
    memorywidget.h \
    pictureitem.h \
    plotitem.h \
    plotrenderitem.h \
    qgetoptions.h \
    vectorcurverenderitem.h \
    sessionmodel.h \
    svgitem.h \
    stroketab.h \
    tabwidget.h \
    vectoreditordialog.h \
    vectormodel.h \
    vectortablemodel.h \
    viewcommand.h \
    view.h \
    viewgridlayout.h \
    viewitemdialog.h \
    viewitem.h \
    viewmanager.h

FORMS += \
    aboutdialog.ui \
    datamanager.ui \
    dialog.ui \
    dialogpage.ui \
    debugdialog.ui \
    exportgraphicsdialog.ui \
    filltab.ui \
    stroketab.ui \
    vectoreditordialog.ui \
    viewmanager.ui

