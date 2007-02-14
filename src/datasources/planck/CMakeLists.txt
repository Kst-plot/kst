
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_planck_PART_SRCS planck.cpp planckdata.cpp planckobj.cpp )

kde4_automoc(${kstdata_planck_PART_SRCS})

kde4_add_plugin(kstdata_planck ${kstdata_planck_PART_SRCS})

target_link_libraries(kstdata_planck  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_planck DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_planck.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(srcdir)/../../extdate $(PIOINCLUDES) $(TFINCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_planck.la
#
#kstdata_planck_la_LIBADD=$(PIOLIBS) $(TRANSFUNCS)
#kstdata_planck_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_planck_la_SOURCES=planck.cpp planckdata.cpp planckobj.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_planck.desktop
#servicesdir=$(kde_servicesdir)/kst
#
