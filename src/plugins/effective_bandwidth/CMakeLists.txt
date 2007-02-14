
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_effbandwidth_PART_SRCS effective_bandwidth.cpp )

kde4_automoc(${kstobject_effbandwidth_PART_SRCS})

kde4_add_plugin(kstobject_effbandwidth ${kstobject_effbandwidth_PART_SRCS})

target_link_libraries(kstobject_effbandwidth  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_effbandwidth DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_effbandwidth.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_effbandwidth.la
#
#kstobject_effbandwidth_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_effbandwidth_la_SOURCES=effective_bandwidth.cpp
#
#services_DATA=kstobject_effbandwidth.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
