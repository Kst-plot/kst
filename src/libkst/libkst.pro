include($$PWD/../../kst.pri)
TEMPLATE = lib
TARGET = $$qtLibraryTarget(kst2lib)
DESTDIR = $$OUTPUT_DIR/lib
win32:CONFIG += staticlib
!isEmpty(INSTALL_PREFIX) { 
    target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR
    INSTALLS += target
}
INCLUDEPATH += tmp \
    $$OUTPUT_DIR/src/libkst/tmp
SOURCES += builtindatasources.cpp \
    builtinprimitives.cpp \
    coredocument.cpp \
    datacollection.cpp \
    datamatrix.cpp \
    dataprimitive.cpp \
    datasource.cpp \
    datasourcefactory.cpp \
    datasourcepluginfactory.cpp \
    datavector.cpp \
    datascalar.cpp \
    datastring.cpp \
    dateparser.cpp \
    debug.cpp \
    editablematrix.cpp \
    editablevector.cpp \
    extension.cpp \
    generatedmatrix.cpp \
    generatedvector.cpp \
    math_kst.cpp \
    matrix.cpp \
    matrixfactory.cpp \
    namedobject.cpp \
    object.cpp \
    objectlist.cpp \
    objectmap.cpp \
    objectstore.cpp \
    plotiteminterface.cpp \
    primitive.cpp \
    primitivefactory.cpp \
    rwlock.cpp \
    scalar.cpp \
    scalarfactory.cpp \
    shortnameindex.cpp \
    string_kst.cpp \
    stringfactory.cpp \
    updatemanager.cpp \
    vector.cpp \
    vectorfactory.cpp \
    vscalar.cpp 
!win32:SOURCES += stdinsource.cpp
!macx:!win32:SOURCES += sysinfo.c \
    psversion.c
HEADERS += builtindatasources.h \
    builtinprimitives.h \
    coredocument.h \
    datacollection.h \
    datamatrix.h \
    dataplugin.h \
    dataprimitive.h \
    datasource.h \
    datasourcefactory.h \
    datasourcepluginfactory.h \
    datavector.h \
    datascalar.h \
    datastring.h \
    dateparser.h \
    debug.h \
    editablematrix.h \
    editablevector.h \
    events.h \
    extension.h \
    generatedmatrix.h \
    generatedvector.h \
    kst_export.h \
    kst_i18n.h \
    kstrevision.h \
    ksttimers.h \
    index_kst.h \
    logevents.h \
    math_kst.h \
    matrix.h \
    matrixfactory.h \
    namedobject.h \
    object.h \
    objectlist.h \
    objectmap.h \
    objectstore.h \
    plotiteminterface.h \
    primitive.h \
    primitivefactory.h \
    procps.h \
    psversion.h \
    rwlock.h \
    scalar.h \
    scalarfactory.h \
    sharedptr.h \
    stdinsource.h \
    string_kst.h \
    stringfactory.h \
    sysinfo.h \
    timezones.h \
    updatemanager.h \
    vector.h \
    vectorfactory.h \
    vscalar.h 
