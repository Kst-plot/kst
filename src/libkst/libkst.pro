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
    object.cpp \
    coredocument.cpp \
    datacollection.cpp \
    datasource.cpp \
    extension.cpp \
    objectstore.cpp \
    debug.cpp \
    rwlock.cpp \
    math_kst.cpp \
    dateparser.cpp \
    #  Needs porting, but is unused anyway
    #timezones.cpp \
    scalar.cpp \
    scalarfactory.cpp \
    # $(PROCPS_COPY) \
    $$PROCPS \
    string_kst.cpp \
    stringfactory.cpp \
    matrix.cpp \
    datamatrix.cpp \
    generatedmatrix.cpp \
    editablematrix.cpp \
    vector.cpp \
    generatedvector.cpp \
    editablevector.cpp \
    datavector.cpp \
    vectordefaults.cpp \
    matrixdefaults.cpp \
    plotiteminterface.cpp \
    primitive.cpp \
    primitivefactory.cpp \
    datasourcefactory.cpp \
    objectlist.cpp \
    objectmap.cpp \
    objecttag.cpp \
    builtinprimitives.cpp \
    vectorfactory.cpp

!win32:SOURCES += stdinsource.cpp

HEADERS += \
    datasourcefactory.h \
    coredocument.h \
    datacollection.h \
    editablematrix.h \
    editablevector.h \
    objectstore.h \
    dataplugin.h \
    datasource.h \
    dateparser.h \
    debug.h \
    events.h \
    kst_export.h \
    extension.h \
    kst_i18n.h \
    index_kst.h \
    math_kst.h \
    matrixdefaults.h \
    matrix.h \
    object.h \
    plotiteminterface.h \
    primitive.h \
    kstrevision.h \
    datamatrix.h \
    datavector.h \
    scalar.h \
    scalarfactory.h \
    sharedptr.h \
    generatedmatrix.h \
    string_kst.h \
    stringfactory.h \
    generatedvector.h \
    ksttimers.h \
    timezones.h \
    vectordefaults.h \
    vector.h \
    logevents.h \
    primitivefactory.h \
    procps.h \
    psversion.h \
    rwlock.h \
    stdinsource.h \
    sysinfo.h \
    objectlist.h \
    objectmap.h \
    objecttag.h \
    builtinprimitives.h \
    vectorfactory.h
