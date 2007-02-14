
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_linefit_PART_SRCS linefit.cpp )

kde4_automoc(${kstobject_linefit_PART_SRCS})

kde4_add_plugin(kstobject_linefit ${kstobject_linefit_PART_SRCS})

target_link_libraries(kstobject_linefit  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_linefit DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_linefit.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/widgets -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstapp  -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_linefit.la
#
#kstobject_linefit_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_linefit_la_SOURCES=linefit.cpp
#
#services_DATA=kstobject_linefit.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
