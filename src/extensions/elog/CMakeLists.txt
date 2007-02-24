
include_directories( ${CMAKE_SOURCE_DIR}/src/libkst ${CMAKE_SOURCE_DIR}/src/libkstmath ${CMAKE_SOURCE_DIR}/src/libkstapp ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


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

