
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_lfiio_PART_SRCS lfiio.cpp )

kde4_automoc(${kstdata_lfiio_PART_SRCS})

kde4_add_plugin(kstdata_lfiio ${kstdata_lfiio_PART_SRCS})

target_link_libraries(kstdata_lfiio  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_lfiio DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_lfiio.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES = kstdata_lfiio.la
#
#kstdata_lfiio_la_LDFLAGS = $(all_libraries) -module -avoid-version 
#kstdata_lfiio_la_LIBADD = $(FITSIO_LIBS)
#kstdata_lfiio_la_SOURCES = lfiio.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_lfiio.desktop
#servicesdir=$(kde_servicesdir)/kst
#
