
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_indirect_PART_SRCS indirect.cpp )

kde4_automoc(${kstdata_indirect_PART_SRCS})

kde4_add_plugin(kstdata_indirect ${kstdata_indirect_PART_SRCS})

target_link_libraries(kstdata_indirect  ${KDE4_KDECORE_LIBS} kst)

install(TARGETS kstdata_indirect DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_indirect.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )

