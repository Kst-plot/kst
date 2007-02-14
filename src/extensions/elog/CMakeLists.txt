
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstextension_elog_PART_SRCS 
   elog.cpp 
   elogthread.cpp 
   elogbasicthreadsubmit.cpp 
   elogeventthreadsubmit.cpp 
   elogthreadsubmit.cpp 
   elogthreadattrs.cpp 
   elogentry_i.cpp 
   elogevententry_i.cpp 
   elogconfiguration_i.cpp )

kde4_automoc(${kstextension_elog_PART_SRCS})

kde4_add_ui_files(kstextension_elog_PART_SRCS elogentry.ui elogevententry.ui elogconfiguration.ui )

kde4_add_plugin(kstextension_elog ${kstextension_elog_PART_SRCS})

target_link_libraries(kstextension_elog  ${KDE4_KDECORE_LIBS} kst ${KDE4_KPARTS_LIBS} )

install(TARGETS kstextension_elog DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  addelogentry.png launchelogbrowser.png DESTINATION ${KDE4_ICON_DIR}/hicolor/22x22/actions/ )
install( FILES  kstextension_elog.rc DESTINATION ${DATA_INSTALL_DIR}/kst )
install( FILES  kstextension_elog.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../../libkst -I$(top_srcdir)/kst/src/libkstapp -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstextension_elog.la
#
#kstextension_elog_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstextension_elog_la_LIBADD=../../libkst/libkst.la $(LIB_KPARTS)
#kstextension_elog_la_SOURCES=elog.cpp elogthread.cpp elogbasicthreadsubmit.cpp elogeventthreadsubmit.cpp elogthreadsubmit.cpp elogthreadattrs.cpp elogentry_i.cpp elogevententry_i.cpp elogconfiguration_i.cpp elogentry.ui elogevententry.ui elogconfiguration.ui
#
#METASOURCES=AUTO
#
#services_DATA=kstextension_elog.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#rcdir=$(kde_datadir)/kst
#rc_DATA=kstextension_elog.rc
#
#iconsdir = $(kde_icondir)/hicolor/22x22/actions/
#icons_DATA = addelogentry.png launchelogbrowser.png
#
