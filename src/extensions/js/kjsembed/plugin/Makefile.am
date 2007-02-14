# -*- makefile -*-

# Make sure the include path includes Qt's uilib
INCLUDES= -I$(QTDIR)/tools/designer/uilib -I$(srcdir)/.. -I.. -I$(top_srcdir) $(all_includes)

INCLUDES += -DQT_CLEAN_NAMESPACE -DQT_NO_ASCII_CAST

kde_module_LTLIBRARIES = libjsconsoleplugin.la

METASOURCES = AUTO

libjsconsoleplugin_la_SOURCES = jsconsoleplugin.cpp
libjsconsoleplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
libjsconsoleplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) $(KDE_RPATH)

#pluginsdir   = $(kde_datadir)/konqueror/kpartplugins
#plugins_DATA = plugin_jsconsole.rc



