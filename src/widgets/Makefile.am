lib_LTLIBRARIES = libkstwidgets.la

kde_widget_LTLIBRARIES = kstwidgets.la

libkstwidgets_la_SOURCES = \
	kstbackbuffer.cpp \
	comboboxselection.ui \
	comboboxselection_i.cpp \
	stringeditor.ui \
	scalareditor.ui \
	scalarselector.ui \
	matrixselector.ui \
	vectorselector.ui \
	stringselector.ui \
	editmultiplewidget.ui \
	curveplacementwidget.ui \
	curveappearancewidget.ui \
	fftoptionswidget.ui \
	draggablelistbox.cpp \
	draggablelistview.cpp \
	plotlistbox.cpp \
	vectorlistview.cpp \
	kstcombobox.cpp \
	datarangewidget.ui \
	colorpalettewidget.ui

libkstwidgets_la_LDFLAGS = -version-info @KST_LIBKST_VERSION@ $(all_libraries) -no-undefined
libkstwidgets_la_LIBADD = $(LIB_KIO) ../libkstmath/libkstmath.la

kstwidgets_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
kstwidgets_la_LIBADD = libkstwidgets.la
kstwidgets_la_SOURCES = kstwidgets.cpp

METASOURCES=AUTO

# set the include path for X, qt and KDE
INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I../libkst -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
	fi

kstwidgets.lo: curveappearancewidget.h curveplacementwidget.h datarangewidget.h editmultiplewidget.h fftoptionswidget.h matrixselector.h scalarselector.h stringselector.h vectorselector.h comboboxselection.h
scalarselector.lo: comboboxselection.h
vectorselector.lo: comboboxselection.h
matrixselector.lo: comboboxselection.h
stringselector.lo: comboboxselection.h
comboboxselection_i.lo: comboboxselection.h

