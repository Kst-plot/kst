
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_qimagesource_PART_SRCS qimagesource.cpp )

kde4_automoc(${kstdata_qimagesource_PART_SRCS})

kde4_add_plugin(kstdata_qimagesource ${kstdata_qimagesource_PART_SRCS})

target_link_libraries(kstdata_qimagesource  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_qimagesource DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_qimagesource.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_qimagesource.la
#
#kstdata_qimagesource_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_qimagesource_la_SOURCES=qimagesource.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_qimagesource.desktop
#servicesdir=$(kde_servicesdir)/kst
#
