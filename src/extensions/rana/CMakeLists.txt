
include_directories( ${CMAKE_SOURCE_DIR}/kst ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstextension_rana_PART_SRCS 
   rana.cpp 
   noisetemperaturecalculation_i.cpp 
   receiverinputoffset_i.cpp 
   linearitygainisolation_i.cpp )

kde4_automoc(${kstextension_rana_PART_SRCS})

kde4_add_ui_files(kstextension_rana_PART_SRCS noisetemperaturecalculation.ui receiverinputoffset.ui linearitygainisolation.ui )

kde4_add_plugin(kstextension_rana ${kstextension_rana_PART_SRCS})

target_link_libraries(kstextension_rana  ${KDE4_KDECORE_LIBS} kst ${KDE4_KPARTS_LIBS} )

install(TARGETS kstextension_rana DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  DESTINATION ${KDE4_ICON_DIR}/hicolor/22x22/actions/ )
install( FILES  kstextension_rana.rc DESTINATION ${DATA_INSTALL_DIR}/kst )
install( FILES  kstextension_rana.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir) -I$(top_srcdir)/kst $(KMDI_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstextension_rana.la
#
#kstextension_rana_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstextension_rana_la_LIBADD=../libkst/libkst.la $(LIB_KPARTS) $(GSL_LIBS)
#kstextension_rana_la_SOURCES=rana.cpp noisetemperaturecalculation_i.cpp receiverinputoffset_i.cpp linearitygainisolation_i.cpp noisetemperaturecalculation.ui receiverinputoffset.ui linearitygainisolation.ui
#
#METASOURCES=AUTO
#
#services_DATA=kstextension_rana.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#rcdir=$(kde_datadir)/kst
#rc_DATA=kstextension_rana.rc
#
#iconsdir = $(kde_icondir)/hicolor/22x22/actions/
#icons_DATA = 
