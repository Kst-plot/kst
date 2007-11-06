include($$PWD/../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kst
DESTDIR = $$OUTPUT_DIR/lib
win32:CONFIG += staticlib

INCLUDEPATH += \
    tmp \
    $$OUTPUT_DIR/src/libkst/tmp

x11:!macx:PROCPS += sysinfo.c psversion.c
SOURCES += \
    builtinprimitives.cpp \
    coredocument.cpp \
    datacollection.cpp \
    datamatrix.cpp \
    datasource.cpp \
    datasourcefactory.cpp \
    datavector.cpp \
    dateparser.cpp \
    debug.cpp \
    editablematrix.cpp \
    editablevector.cpp \
    extension.cpp \
    generatedmatrix.cpp \
    generatedvector.cpp \
    math_kst.cpp \
    matrix.cpp \
    matrixdefaults.cpp \
    matrixfactory.cpp \
    object.cpp \
    objectlist.cpp \
    objectmap.cpp \
    objectstore.cpp \
    objecttag.cpp \
    plotiteminterface.cpp \
    primitive.cpp \
    primitivefactory.cpp \
    rwlock.cpp \
    scalar.cpp \
    scalarfactory.cpp \
    string_kst.cpp \
    stringfactory.cpp \
    vector.cpp \
    vectordefaults.cpp \
    vectorfactory.cpp \
    #  Needs porting, but is unused anyway
    #timezones.cpp \
    # $(PROCPS_COPY) \
    $$PROCPS 

!win32:SOURCES += stdinsource.cpp

HEADERS += \
    builtinprimitives.h \
    coredocument.h \
    datacollection.h \
    datamatrix.h \
    dataplugin.h \
    datasource.h \
    datasourcefactory.h \
    datavector.h \
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
    matrixdefaults.h \
    matrixfactory.h \
    object.h \
    objectlist.h \
    objectmap.h \
    objectstore.h \
    objecttag.h \
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
    vector.h \
    vectordefaults.h \
    vectorfactory.h
