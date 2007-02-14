
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_akima_periodic_PART_SRCS akima_periodic.cpp )

kde4_automoc(${kstobject_akima_periodic_PART_SRCS})

kde4_add_plugin(kstobject_akima_periodic ${kstobject_akima_periodic_PART_SRCS})

target_link_libraries(kstobject_akima_periodic  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_akima_periodic DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_akima_periodic.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_akima_periodic.la
#
#kstobject_akima_periodic_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_akima_periodic_la_SOURCES=akima_periodic.cpp
#kstobject_akima_periodic_la_LIBADD = $(GSL_LIBS)
#
#services_DATA=kstobject_akima_periodic.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
