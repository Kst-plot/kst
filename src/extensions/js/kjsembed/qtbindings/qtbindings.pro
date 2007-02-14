include(../qjsembed.pri)

HEADERS += qcombobox_imp.h \
	   qframe_imp.h \
	   qlistviewitem_imp.h \
           qt_imp.h \
           qchecklistitem_imp.h

SOURCES += \
 qlistviewitem_imp.cpp \
 qcombobox_imp.cpp \
 qframe_imp.cpp \
 qt_imp.cpp \
 qchecklistitem_imp.cpp \
 qcanvasellipse_imp.cpp \
 qcanvas_imp.cpp \
 qcanvasitem_imp.cpp \
 qcanvasitemlist_imp.cpp \
 qcanvasline_imp.cpp \
 qcanvaspixmaparray_imp.cpp \
 qcanvaspixmap_imp.cpp \
 qcanvaspolygonalitem_imp.cpp \
 qcanvaspolygon_imp.cpp \
 qcanvasrectangle_imp.cpp \
 qcanvasspline_imp.cpp \
 qcanvassprite_imp.cpp \
 qcanvastext_imp.cpp \
 qcanvasview_imp.cpp

INCLUDEPATH += .. . ../..
CONFIG += staticlib 
TEMPLATE = lib 
