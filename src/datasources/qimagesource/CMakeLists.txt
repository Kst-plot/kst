
include_directories( ${CMAKE_SOURCE_DIR}/src/libkst ${CMAKE_SOURCE_DIR}/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_qimagesource_PART_SRCS qimagesource.cpp )

kde4_automoc(${kstdata_qimagesource_PART_SRCS})

kde4_add_plugin(kstdata_qimagesource ${kstdata_qimagesource_PART_SRCS})

target_link_libraries(kstdata_qimagesource  ${KDE4_KDECORE_LIBS} kst)

install(TARGETS kstdata_qimagesource DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_qimagesource.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )


