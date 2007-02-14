
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_healpix_PART_SRCS 
   healpix.cpp 
   healpix_tools_pix.cpp 
   healpix_tools_proj.cpp 
   healpix_tools_fits.cpp )

kde4_automoc(${kstdata_healpix_PART_SRCS})

kde4_add_ui_files(kstdata_healpix_PART_SRCS healpixconfig.ui )

kde4_add_plugin(kstdata_healpix ${kstdata_healpix_PART_SRCS})

target_link_libraries(kstdata_healpix  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_healpix DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_healpix.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_healpix.la
#
#kstdata_healpix_la_LIBADD=$(FITSIO_LIBS)
#kstdata_healpix_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_healpix_la_SOURCES=healpix.cpp healpix_tools_pix.cpp healpix_tools_proj.cpp healpix_tools_fits.cpp healpixconfig.ui
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_healpix.desktop
#servicesdir=$(kde_servicesdir)/kst
#
