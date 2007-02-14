
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_statistics_PART_SRCS statistics.cpp )

kde4_automoc(${kstobject_statistics_PART_SRCS})

kde4_add_plugin(kstobject_statistics ${kstobject_statistics_PART_SRCS})

target_link_libraries(kstobject_statistics  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_statistics DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_statistics.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_statistics.la
#
#kstobject_statistics_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_statistics_la_SOURCES=statistics.cpp
#
#services_DATA=kstobject_statistics.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
