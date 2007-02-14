
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_testplugin_PART_SRCS testplugin.cpp )

kde4_automoc(${kstobject_testplugin_PART_SRCS})

kde4_add_plugin(kstobject_testplugin ${kstobject_testplugin_PART_SRCS})

target_link_libraries(kstobject_testplugin  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_testplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_testplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_testplugin.la
#
#kstobject_testplugin_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_testplugin_la_SOURCES=testplugin.cpp
#
#services_DATA=kstobject_testplugin.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
