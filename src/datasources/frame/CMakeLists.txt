
include_directories( ${CMAKE_SOURCE_DIR}/src/libkst ${CMAKE_SOURCE_DIR}/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_frame_PART_SRCS frame.cpp readdata.c creaddata.c )

kde4_automoc(${kstdata_frame_PART_SRCS})

kde4_add_plugin(kstdata_frame ${kstdata_frame_PART_SRCS})

target_link_libraries(kstdata_frame  ${KDE4_KDECORE_LIBS} kst)

install(TARGETS kstdata_frame DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_frame.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )

