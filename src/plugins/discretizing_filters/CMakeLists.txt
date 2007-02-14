
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_discretizing_filters_PART_SRCS genericfilter.cpp )

kde4_automoc(${kstobject_discretizing_filters_PART_SRCS})

kde4_add_plugin(kstobject_discretizing_filters ${kstobject_discretizing_filters_PART_SRCS})

target_link_libraries(kstobject_discretizing_filters  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_discretizing_filters DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_discretizing_filters.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_discretizing_filters.la
#
#kstobject_discretizing_filters_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_discretizing_filters_la_SOURCES=genericfilter.cpp
#
#services_DATA=kstobject_discretizing_filters.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
