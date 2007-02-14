
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstextension_pulse_PART_SRCS pulse.cpp kstgvector.cpp )

kde4_automoc(${kstextension_pulse_PART_SRCS})

kde4_add_ui_files(kstextension_pulse_PART_SRCS kstpulsegenerator.ui )

kde4_add_plugin(kstextension_pulse ${kstextension_pulse_PART_SRCS})

target_link_libraries(kstextension_pulse  ${KDE4_KDECORE_LIBS} kst ${KDE4_KPARTS_LIBS} )

install(TARGETS kstextension_pulse DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  DESTINATION ${KDE4_ICON_DIR}/hicolor/22x22/actions/ )
install( FILES  kstextension_pulse.rc DESTINATION ${DATA_INSTALL_DIR}/kst )
install( FILES  kstextension_pulse.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate -I$(top_srcdir) -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/libkstapp $(KMDI_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstextension_pulse.la
#
#kstextension_pulse_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstextension_pulse_la_LIBADD=../../libkst/libkst.la $(LIB_KPARTS)
#kstextension_pulse_la_SOURCES=pulse.cpp kstpulsegenerator.ui kstgvector.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstextension_pulse.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#rcdir=$(kde_datadir)/kst
#rc_DATA=kstextension_pulse.rc
#
#iconsdir = $(kde_icondir)/hicolor/22x22/actions/
#icons_DATA = 
