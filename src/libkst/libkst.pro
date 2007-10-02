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
    kstobject.cpp \
    kstdatasource.cpp \
    kstextension.cpp \
    kstdatacollection.cpp \
    kstdebug.cpp \
    rwlock.cpp \
    kstmath.cpp \
    kstdateparser.cpp \
    #  Needs porting, but is unused anyway
    #ksttimezones.cpp \
    kstscalar.cpp \
    # $(PROCPS_COPY) \
    $$PROCPS \
    kststring.cpp \
    kstmatrix.cpp \
    datamatrix.cpp \
    generatedmatrix.cpp \
    editablematrix.cpp \
    kstvector.cpp \
    kstsvector.cpp \
    editablevector.cpp \
    datavector.cpp \
    kstvectordefaults.cpp \
    kstmatrixdefaults.cpp \
    kstprimitive.cpp \
    defaultprimitivenames.cpp \
    primitivefactory.cpp \
    datasourcefactory.cpp \
    kstobjectcollection.cpp \
    kstobjectlist.cpp \
    kstobjectmap.cpp \
    kstobjecttag.cpp \
    builtinprimitives.cpp \
    vectorfactory.cpp

!win32:SOURCES += stdinsource.cpp

HEADERS += \
    datasourcefactory.h \
    defaultprimitivenames.h \
    editablematrix.h \
    editablevector.h \
    kstdatacollection.h \
    kstdataplugin.h \
    kstdatasource.h \
    kstdateparser.h \
    kstdebug.h \
    kstevents.h \
    kst_export.h \
    kstextension.h \
    kst_i18n.h \
    kstindex.h \
    kstmath.h \
    kstmatrixdefaults.h \
    kstmatrix.h \
    kstobjectcollection.h \
    kstobject.h \
    kstprimitive.h \
    kstrevision.h \
    datamatrix.h \
    datavector.h \
    kstscalar.h \
    kstsharedptr.h \
    generatedmatrix.h \
    kststring.h \
    kstsvector.h \
    ksttimers.h \
    ksttimezones.h \
    kstvectordefaults.h \
    kstvector.h \
    logevents.h \
    primitivefactory.h \
    procps.h \
    psversion.h \
    rwlock.h \
    stdinsource.h \
    sysinfo.h \
    kstobjectcollection.h \
    kstobjectlist.h \
    kstobjectmap.h \
    kstobjecttag.h \
    builtinprimitives.h \
    vectorfactory.h
