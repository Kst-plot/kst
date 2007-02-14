
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_cspline_periodic_PART_SRCS cspline_periodic.cpp )

kde4_automoc(${kstobject_cspline_periodic_PART_SRCS})

kde4_add_plugin(kstobject_cspline_periodic ${kstobject_cspline_periodic_PART_SRCS})

target_link_libraries(kstobject_cspline_periodic  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_cspline_periodic DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_cspline_periodic.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_cspline_periodic.la
#
#kstobject_cspline_periodic_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_cspline_periodic_la_SOURCES=cspline_periodic.cpp
#kstobject_cspline_periodic_la_LIBADD = $(GSL_LIBS)
#
#services_DATA=kstobject_cspline_periodic.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
