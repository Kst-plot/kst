
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_wmap_PART_SRCS wmap.cpp )

kde4_automoc(${kstdata_wmap_PART_SRCS})

kde4_add_plugin(kstdata_wmap ${kstdata_wmap_PART_SRCS})

target_link_libraries(kstdata_wmap  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_wmap DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_wmap.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES = kstdata_wmap.la
#
#kstdata_wmap_la_LDFLAGS = $(all_libraries) -module -avoid-version 
#kstdata_wmap_la_LIBADD = $(FITSIO_LIBS)
#kstdata_wmap_la_SOURCES = wmap.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_wmap.desktop
#servicesdir=$(kde_servicesdir)/kst
#
