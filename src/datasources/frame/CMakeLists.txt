
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_frame_PART_SRCS frame.cpp readdata.c creaddata.c )

kde4_automoc(${kstdata_frame_PART_SRCS})

kde4_add_plugin(kstdata_frame ${kstdata_frame_PART_SRCS})

target_link_libraries(kstdata_frame  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_frame DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_frame.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_frame.la
#
#kstdata_frame_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_frame_la_SOURCES=frame.cpp readdata.c creaddata.c
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_frame.desktop
#servicesdir=$(kde_servicesdir)/kst
#
