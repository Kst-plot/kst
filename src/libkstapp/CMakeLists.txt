include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_BINARY_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

set(kstapp_LIB_SRCS
   kstapplication.cpp
   kstmainwindow.cpp
   view.cpp
   viewitem.cpp
   boxitem.cpp
   lineitem.cpp
   labelitem.cpp
   viewcommand.cpp
   )

kde4_automoc(${kstapp_LIB_SRCS})

kde4_add_library(kstapp SHARED ${kstapp_LIB_SRCS})

target_link_libraries(kstapp kstmath)

set_target_properties(kstapp PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2)
install(TARGETS kstapp DESTINATION ${LIB_INSTALL_DIR} )
