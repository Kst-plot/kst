
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

#
#INCLUDES= -I$(srcdir)/.. -I$(srcdir)/../.. -I$(srcdir)/../../../../libkst $(all_includes)
#KDE_CXXFLAGS = -DQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE -Wno-unused
#
#noinst_LTLIBRARIES = libkjsembedqtbindings.la
#
#libkjsembedqtbindings_la_SOURCES =	qlistviewitem_imp.cpp \
#					qcombobox_imp.cpp \
#					qframe_imp.cpp \
#					qt_imp.cpp \
#					qchecklistitem_imp.cpp \
#					qpopupmenu_imp.cpp \
#					qmenudata_imp.cpp \
#					qmenuitem_imp.cpp
#
#libkjsembedqtbindings_la_LDFLAGS =	$(all_libraries) $(KDE_RPATH)
#libkjsembedqtbindings_la_LIBADD =	$(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)
#
#libkjsembedqtbindings_la_METASOURCES = AUTO
#
#
