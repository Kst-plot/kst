
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_ascii_PART_SRCS ascii.cpp )

kde4_automoc(${kstdata_ascii_PART_SRCS})

kde4_add_ui_files(kstdata_ascii_PART_SRCS asciiconfig.ui )

kde4_add_plugin(kstdata_ascii ${kstdata_ascii_PART_SRCS})

target_link_libraries(kstdata_ascii  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_ascii DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_ascii.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstdata_ascii.la
#
#kstdata_ascii_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstdata_ascii_la_SOURCES=ascii.cpp asciiconfig.ui
#
#METASOURCES=AUTO
#
#services_DATA=kstdata_ascii.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kst.pot
