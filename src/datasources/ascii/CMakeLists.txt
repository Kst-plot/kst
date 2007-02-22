
include_directories( ${CMAKE_SOURCE_DIR}/src/libkst ${CMAKE_SOURCE_DIR}/src/libkstmath ${CMAKE_SOURCE_DIR}/src/datasources/ascii ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_ascii_PART_SRCS ascii.cpp )

kde4_automoc(${kstdata_ascii_PART_SRCS})

kde4_add_ui_files(kstdata_ascii_PART_SRCS asciiconfig.ui )

kde4_add_plugin(kstdata_ascii ${kstdata_ascii_PART_SRCS})

target_link_libraries(kstdata_ascii  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstdata_ascii DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_ascii.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )

