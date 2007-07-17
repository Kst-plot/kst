include($$PWD/../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kst
DESTDIR = $$OUTPUT_DIR/lib

INCLUDEPATH += \
    tmp \
    $$OUTPUT_DIR/src/libkst/tmp

SOURCES += \
    kstobject.cpp \
    kstdatasource.cpp \
    stdinsource.cpp \
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
    # FIXME: switch to a variable
    sysinfo.c \
    psversion.c \
    # END FIXME
    kststring.cpp \
    kstmatrix.cpp \
    kstrmatrix.cpp \
    kstsmatrix.cpp \
    kstamatrix.cpp \
    kstvector.cpp \
    kstsvector.cpp \
    kstavector.cpp \
    kstrvector.cpp \
    kstvectordefaults.cpp \
    kstmatrixdefaults.cpp \
    kstprimitive.cpp \
    defaultprimitivenames.cpp \
    primitivefactory.cpp \
    datasourcefactory.cpp \
    kstobjectcollection.cpp \
    kstobjectlist.cpp \
    kstobjectmap.cpp \
    kstobjecttag.cpp

HEADERS += \
    datasourcefactory.h \
    defaultprimitivenames.h \
    kstamatrix.h \
    kstavector.h \
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
    kstrmatrix.h \
    kstrvector.h \
    kstscalar.h \
    kstsharedptr.h \
    kstsmatrix.h \
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
    kstobjecttag.h
