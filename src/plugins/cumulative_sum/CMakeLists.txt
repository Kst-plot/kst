
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_cumulative_sum_PART_SRCS cumulative_sum.cpp )

kde4_automoc(${kstobject_cumulative_sum_PART_SRCS})

kde4_add_plugin(kstobject_cumulative_sum ${kstobject_cumulative_sum_PART_SRCS})

target_link_libraries(kstobject_cumulative_sum  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_cumulative_sum DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_cumulative_sum.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_cumulative_sum.la
#
#kstobject_cumulative_sum_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_cumulative_sum_la_SOURCES=cumulative_sum.cpp
#
#services_DATA=kstobject_cumulative_sum.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
