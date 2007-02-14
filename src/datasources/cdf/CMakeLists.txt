
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_cdf_PART_SRCS cdf.cpp )

kde4_automoc(${kstdata_cdf_PART_SRCS})

kde4_add_plugin(kstdata_cdf ${kstdata_cdf_PART_SRCS})

target_link_libraries(kstdata_cdf  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_cdf DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_cdf.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(CDFINCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_cdf.la
#
#kstdata_cdf_la_LIBADD=$(CDFLIBS)
#kstdata_cdf_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_cdf_la_SOURCES=cdf.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_cdf.desktop
#servicesdir=$(kde_servicesdir)/kst
#
