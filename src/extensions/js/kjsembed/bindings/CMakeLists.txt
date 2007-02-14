
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

#
#INCLUDES= -I$(srcdir)/.. -I$(srcdir)/../.. -I$(srcdir)/../../../../libkst $(all_includes)
#KDE_CXXFLAGS = -DQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE -Wno-unused
#
#noinst_LTLIBRARIES = libkjsembedbindings.la
#
#libkjsembedbindings_la_SOURCES =	image_imp.cpp \
#					dcop_imp.cpp \
#					painter_imp.cpp \
#					pixmap_imp.cpp \
#					netaccess_imp.cpp \
#					kconfig_imp.cpp \
#					bindingobject.cpp \
#					movie_imp.cpp \
#					sql_imp.cpp \
#					pen_imp.cpp \
#					brush_imp.cpp \
#					iconset_imp.cpp \
#					point_imp.cpp \
#					rect_imp.cpp \
#					size_imp.cpp
#
#libkjsembedbindings_la_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
#libkjsembedbindings_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS) \
#				$(LIB_KFILE)  $(LIBSOCKET)
#
#libkjsembedbindings_la_METASOURCES = AUTO
#
#
