
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_naddirect_PART_SRCS naddirect.cpp nadconnection.cpp )

kde4_automoc(${kstdata_naddirect_PART_SRCS})

kde4_add_plugin(kstdata_naddirect ${kstdata_naddirect_PART_SRCS})

target_link_libraries(kstdata_naddirect  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} curl )

install(TARGETS kstdata_naddirect DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_naddirect.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_naddirect.la
#
#kstdata_naddirect_la_LIBADD=$(LIB_KIO) -lcurl
#kstdata_naddirect_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_naddirect_la_SOURCES=naddirect.cpp nadconnection.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_naddirect.desktop
#servicesdir=$(kde_servicesdir)/kst
#
