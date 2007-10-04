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
    datasource.cpp \
    kstextension.cpp \
    datacollection.cpp \
    debug.cpp \
    rwlock.cpp \
    kstmath.cpp \
    dateparser.cpp \
    #  Needs porting, but is unused anyway
    #ksttimezones.cpp \
    scalar.cpp \
    # $(PROCPS_COPY) \
    $$PROCPS \
    kststring.cpp \
    matrix.cpp \
    datamatrix.cpp \
    generatedmatrix.cpp \
    editablematrix.cpp \
    vector.cpp \
    generatedvector.cpp \
    editablevector.cpp \
    datavector.cpp \
    kstvectordefaults.cpp \
    matrixdefaults.cpp \
    kstprimitive.cpp \
    defaultprimitivenames.cpp \
    primitivefactory.cpp \
    datasourcefactory.cpp \
    objectcollection.cpp \
    objectlist.cpp \
    objectmap.cpp \
    objecttag.cpp \
    builtinprimitives.cpp \
    vectorfactory.cpp

!win32:SOURCES += stdinsource.cpp

HEADERS += \
    datasourcefactory.h \
    defaultprimitivenames.h \
    editablematrix.h \
    editablevector.h \
    datacollection.h \
    dataplugin.h \
    datasource.h \
    dateparser.h \
    debug.h \
    kstevents.h \
    kst_export.h \
    kstextension.h \
    kst_i18n.h \
    kstindex.h \
    kstmath.h \
    matrixdefaults.h \
    matrix.h \
    objectcollection.h \
    object.h \
    kstprimitive.h \
    kstrevision.h \
    datamatrix.h \
    datavector.h \
    scalar.h \
    kstsharedptr.h \
    generatedmatrix.h \
    kststring.h \
    generatedvector.h \
    ksttimers.h \
    ksttimezones.h \
    kstvectordefaults.h \
    vector.h \
    logevents.h \
    primitivefactory.h \
    procps.h \
    psversion.h \
    rwlock.h \
    stdinsource.h \
    sysinfo.h \
    objectcollection.h \
    objectlist.h \
    objectmap.h \
    objecttag.h \
    builtinprimitives.h \
    vectorfactory.h
