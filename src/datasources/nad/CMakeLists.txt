
add_subdirectory( nadslave )

include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_nad_PART_SRCS nadsource.cpp )

kde4_automoc(${kstdata_nad_PART_SRCS})

kde4_add_plugin(kstdata_nad ${kstdata_nad_PART_SRCS})

target_link_libraries(kstdata_nad  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_nad DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_nad.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#SUBDIRS=nadslave
#
#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(NADINCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_nad.la
#
#kstdata_nad_la_LIBADD=$(NADLIBS)
#kstdata_nad_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_nad_la_SOURCES=nadsource.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_nad.desktop
#servicesdir=$(kde_servicesdir)/kst
#
