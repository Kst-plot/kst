
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(jsconsoleplugin_PART_SRCS jsconsoleplugin.cpp )

kde4_automoc(${jsconsoleplugin_PART_SRCS})

kde4_add_plugin(jsconsoleplugin WITH_PREFIX ${jsconsoleplugin_PART_SRCS})

target_link_libraries(jsconsoleplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kjsembed )

install(TARGETS jsconsoleplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############




#original Makefile.am contents follow:

## -*- makefile -*-
#
## Make sure the include path includes Qt's uilib
#INCLUDES= -I$(QTDIR)/tools/designer/uilib -I$(srcdir)/.. -I.. -I$(top_srcdir) $(all_includes)
#
#INCLUDES += -DQT_CLEAN_NAMESPACE -DQT_NO_ASCII_CAST
#
#kde_module_LTLIBRARIES = libjsconsoleplugin.la
#
#METASOURCES = AUTO
#
#libjsconsoleplugin_la_SOURCES = jsconsoleplugin.cpp
#libjsconsoleplugin_la_LIBADD  = $(LIB_KPARTS) ../libkjsembed.la
#libjsconsoleplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) $(KDE_RPATH)
#
##pluginsdir   = $(kde_datadir)/konqueror/kpartplugins
##plugins_DATA = plugin_jsconsole.rc
#
#
#
