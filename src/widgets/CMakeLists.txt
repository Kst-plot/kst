
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstwidgets_LIB_SRCS
   kstwidgets.cpp
   colorbutton.cpp
   fillandstroke.cpp
   gradienteditor.cpp
   )

kde4_automoc(${kstwidgets_LIB_SRCS})

kde4_add_ui_files(kstwidgets_LIB_SRCS
   fillandstroke.ui
   )

kde4_add_library(kstwidgets SHARED ${kstwidgets_LIB_SRCS})

target_link_libraries(kstwidgets ${KDE4_KDECORE_LIBS} kstmath)

set_target_properties(kstwidgets PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2)
install(TARGETS kstwidgets DESTINATION ${LIB_INSTALL_DIR})
