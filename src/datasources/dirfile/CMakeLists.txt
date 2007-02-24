
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstdata_dirfile_PART_SRCS dirfile.cpp getdata.c )

kde4_automoc(${kstdata_dirfile_PART_SRCS})

kde4_add_plugin(kstdata_dirfile ${kstdata_dirfile_PART_SRCS})

target_link_libraries(kstdata_dirfile  ${KDE4_KDECORE_LIBS} kst)

install(TARGETS kstdata_dirfile DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstdata_dirfile.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )


