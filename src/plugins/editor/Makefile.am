INCLUDES= $(all_includes)

bin_PROGRAMS = kstplugineditor
kstplugineditor_SOURCES = plugineditor.ui main.cpp
kstplugineditor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kstplugineditor_LDADD = $(LIB_KDECORE)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kstplugineditor.pot
