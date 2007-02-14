INCLUDES=  -I$(srcdir)/.. -I$(srcdir)/../.. $(all_includes)
KDE_CXXFLAGS = -DQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE

kde_module_LTLIBRARIES = libcustomobjectplugin.la libcustomqobjectplugin.la \
	libimagefxplugin.la libqprocessplugin.la libfileitemplugin.la
METASOURCES = AUTO

# the Plugin's source, library search path, and link libraries
libcustomobjectplugin_la_SOURCES = customobject_plugin.cpp
libcustomobjectplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libcustomobjectplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la

# the Plugin's source, library search path, and link libraries
libcustomqobjectplugin_la_SOURCES = customqobject_plugin.cpp
libcustomqobjectplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libcustomqobjectplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la

# the Plugin's source, library search path, and link libraries
libimagefxplugin_la_SOURCES = imagefx_plugin.cpp
libimagefxplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libimagefxplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la

# the Plugin's source, library search path, and link libraries
libqprocessplugin_la_SOURCES = qprocess_plugin.cpp
libqprocessplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libqprocessplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la

# the Plugin's source, library search path, and link libraries
libfileitemplugin_la_SOURCES = kfileitemloader.cpp
libfileitemplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libfileitemplugin_la_LIBADD = $(LIB_KPARTS) ../libkjsembed.la


# this is where the desktop file will go
plugindesktopdir   = $(kde_servicesdir)
plugindesktop_DATA = customobject_plugin.desktop customqobject_plugin.desktop imagefx_plugin.desktop\
	 qprocess_plugin.desktop kfileitem_plugin.desktop

