
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_netcdf_PART_SRCS netcdf.cpp )

kde4_automoc(${kstdata_netcdf_PART_SRCS})

kde4_add_plugin(kstdata_netcdf ${kstdata_netcdf_PART_SRCS})

target_link_libraries(kstdata_netcdf  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_netcdf DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_netcdf.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(NETCDFINCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_netcdf.la
#
#kstdata_netcdf_la_LIBADD=$(NETCDFLIBS)
#kstdata_netcdf_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_netcdf_la_SOURCES=netcdf.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_netcdf.desktop
#servicesdir=$(kde_servicesdir)/kst
#
