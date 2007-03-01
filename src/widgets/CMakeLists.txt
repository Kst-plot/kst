
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstwidgets_LIB_SRCS
   kstwidgets.cpp

   colorpalettewidget.cpp
   comboboxselection.cpp
   curveappearancewidget.cpp
   curveplacementwidget.cpp
   kstdatarange.cpp
   editmultiplewidget.cpp
   fftoptionswidget.cpp
   matrixselector.cpp
   scalareditor.cpp
   scalarselector.cpp
   stringeditor.cpp
   stringselector.cpp
   vectorselector.cpp

   kstbackbuffer.cpp
   draggablelistbox.cpp
   draggablelistview.cpp
   plotlistbox.cpp
   vectorlistview.cpp
   kstcombobox.cpp
   )

kde4_automoc(${kstwidgets_LIB_SRCS})

kde4_add_ui_files(kstwidgets_LIB_SRCS
   colorpalettewidget4.ui
   comboboxselection4.ui
   curveappearancewidget4.ui
   curveplacementwidget4.ui
   kstdatarange4.ui
   editmultiplewidget4.ui
   fftoptionswidget4.ui
   matrixselector4.ui
   scalareditor4.ui
   scalarselector4.ui
   stringeditor4.ui
   stringselector4.ui
   vectorselector4.ui
   )

kde4_add_library(kstwidgets SHARED ${kstwidgets_LIB_SRCS})

target_link_libraries(kstwidgets ${KDE4_KDECORE_LIBS} kstmath)

set_target_properties(kstwidgets PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 1)
install(TARGETS kstwidgets DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(kstwidgets_PART_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

kde4_automoc(${kstwidgets_PART_SRCS})

kde4_add_plugin(kstwidgets ${kstwidgets_PART_SRCS})

target_link_libraries(kstwidgets ${KDE4_KDECORE_LIBS})

install(TARGETS kstwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer)


########### install files ###############




#original Makefile.am contents follow:

#lib_LTLIBRARIES = libkstwidgets.la
#
#kde_widget_LTLIBRARIES = kstwidgets.la
#
#libkstwidgets_la_SOURCES = \
#	kstbackbuffer.cpp \
#	comboboxselection.ui \
#	comboboxselection_i.cpp \
#	stringeditor.ui \
#	scalareditor.ui \
#	scalarselector.ui \
#	matrixselector.ui \
#	vectorselector.ui \
#	stringselector.ui \
#	editmultiplewidget.ui \
#	curveplacementwidget.ui \
#	curveappearancewidget.ui \
#	fftoptionswidget.ui \
#	draggablelistbox.cpp \
#	draggablelistview.cpp \
#	plotlistbox.cpp \
#	vectorlistview.cpp \
#	kstcombobox.cpp \
#	datarangewidget.ui \
#	colorpalettewidget.ui
#
#libkstwidgets_la_LDFLAGS = -version-info @KST_LIBKST_VERSION@ $(all_libraries) -no-undefined
#libkstwidgets_la_LIBADD = $(LIB_KIO) ../libkstmath/libkstmath.la
#
#kstwidgets_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
#kstwidgets_la_LIBADD = libkstwidgets.la
#kstwidgets_la_SOURCES = kstwidgets.cpp
#
#METASOURCES=AUTO
#
## set the include path for X, qt and KDE
#INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I../libkst -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)
#
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
#	fi
#
#kstwidgets.lo: curveappearancewidget.h curveplacementwidget.h datarangewidget.h editmultiplewidget.h fftoptionswidget.h matrixselector.h scalarselector.h stringselector.h vectorselector.h comboboxselection.h
#scalarselector.lo: comboboxselection.h
#vectorselector.lo: comboboxselection.h
#matrixselector.lo: comboboxselection.h
#stringselector.lo: comboboxselection.h
#comboboxselection_i.lo: comboboxselection.h
#
