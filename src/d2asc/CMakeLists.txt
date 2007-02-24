
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(d2asc_SRCS d2asc.cpp )

kde4_automoc(${d2asc_SRCS})

kde4_add_executable(d2asc ${d2asc_SRCS})

target_link_libraries(d2asc  ${KDE4_KDECORE_LIBS} kstmath )

install(TARGETS d2asc DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

