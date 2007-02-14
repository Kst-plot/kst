
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(customobjectplugin_PART_SRCS customobject_plugin.cpp )

kde4_automoc(${customobjectplugin_PART_SRCS})

kde4_add_plugin(customobjectplugin WITH_PREFIX ${customobjectplugin_PART_SRCS})

target_link_libraries(customobjectplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS customobjectplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(customqobjectplugin_PART_SRCS customqobject_plugin.cpp )

kde4_automoc(${customqobjectplugin_PART_SRCS})

kde4_add_plugin(customqobjectplugin WITH_PREFIX ${customqobjectplugin_PART_SRCS})

target_link_libraries(customqobjectplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS customqobjectplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(imagefxplugin_PART_SRCS imagefx_plugin.cpp )

kde4_automoc(${imagefxplugin_PART_SRCS})

kde4_add_plugin(imagefxplugin WITH_PREFIX ${imagefxplugin_PART_SRCS})

target_link_libraries(imagefxplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS imagefxplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(qprocessplugin_PART_SRCS qprocess_plugin.cpp )

kde4_automoc(${qprocessplugin_PART_SRCS})

kde4_add_plugin(qprocessplugin WITH_PREFIX ${qprocessplugin_PART_SRCS})

target_link_libraries(qprocessplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS qprocessplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(fileitemplugin_PART_SRCS kfileitemloader.cpp )

kde4_automoc(${fileitemplugin_PART_SRCS})

kde4_add_plugin(fileitemplugin WITH_PREFIX ${fileitemplugin_PART_SRCS})

target_link_libraries(fileitemplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS fileitemplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  customobject_plugin.desktop customqobject_plugin.desktop imagefx_plugin.desktop 	 qprocess_plugin.desktop kfileitem_plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



#original Makefile.am contents follow:

#INCLUDES=  -I$(srcdir)/.. -I$(srcdir)/../.. $(all_includes)
#KDE_CXXFLAGS = -DQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE
#
#kde_module_LTLIBRARIES = libcustomobjectplugin.la libcustomqobjectplugin.la \
#	libimagefxplugin.la libqprocessplugin.la libfileitemplugin.la
#METASOURCES = AUTO
#
## the Plugin's source, library search path, and link libraries
#libcustomobjectplugin_la_SOURCES = customobject_plugin.cpp
#libcustomobjectplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
#libcustomobjectplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
#
## the Plugin's source, library search path, and link libraries
#libcustomqobjectplugin_la_SOURCES = customqobject_plugin.cpp
#libcustomqobjectplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
#libcustomqobjectplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
#
## the Plugin's source, library search path, and link libraries
#libimagefxplugin_la_SOURCES = imagefx_plugin.cpp
#libimagefxplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
#libimagefxplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
#
## the Plugin's source, library search path, and link libraries
#libqprocessplugin_la_SOURCES = qprocess_plugin.cpp
#libqprocessplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
#libqprocessplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
#
## the Plugin's source, library search path, and link libraries
#libfileitemplugin_la_SOURCES = kfileitemloader.cpp
#libfileitemplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#libfileitemplugin_la_LIBADD = $(LIB_KPARTS) ../libkjsembed.la
#
#
## this is where the desktop file will go
#plugindesktopdir   = $(kde_servicesdir)
#plugindesktop_DATA = customobject_plugin.desktop customqobject_plugin.desktop imagefx_plugin.desktop\
#	 qprocess_plugin.desktop kfileitem_plugin.desktop
#
