
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_crosscorrelate_PART_SRCS crosscorrelate.cpp )

kde4_automoc(${kstobject_crosscorrelate_PART_SRCS})

kde4_add_plugin(kstobject_crosscorrelate ${kstobject_crosscorrelate_PART_SRCS})

target_link_libraries(kstobject_crosscorrelate  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_crosscorrelate DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_crosscorrelate.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_crosscorrelate.la
#
#kstobject_crosscorrelate_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_crosscorrelate_la_SOURCES=crosscorrelate.cpp
#kstobject_crosscorrelate_la_LIBADD = $(GSL_LIBS)
#
#services_DATA=kstobject_crosscorrelate.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
