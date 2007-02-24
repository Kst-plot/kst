
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(d2d_SRCS d2d.cpp )

kde4_automoc(${d2d_SRCS})

kde4_add_executable(d2d ${d2d_SRCS})

target_link_libraries(d2d  ${KDE4_KDECORE_LIBS} kstmath )

install(TARGETS d2d DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

