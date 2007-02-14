
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_dirfile_PART_SRCS dirfile.cpp getdata.c )

kde4_automoc(${kstdata_dirfile_PART_SRCS})

kde4_add_plugin(kstdata_dirfile ${kstdata_dirfile_PART_SRCS})

target_link_libraries(kstdata_dirfile  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_dirfile DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_dirfile.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_dirfile.la
#
#kstdata_dirfile_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_dirfile_la_SOURCES=dirfile.cpp getdata.c
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_dirfile.desktop
#servicesdir=$(kde_servicesdir)/kst
#
