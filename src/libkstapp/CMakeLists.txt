include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_BINARY_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

set(kstapp_LIB_SRCS
   applicationsettings.cpp
   boxitem.cpp
   datamanager.cpp
   document.cpp
   ellipseitem.cpp
   kstapplication.cpp
   labelitem.cpp
   lineitem.cpp
   mainwindow.cpp
   matrixmodel.cpp
   pictureitem.cpp
   plotitem.cpp
   sessionmodel.cpp
   tabwidget.cpp
   vectoreditordialog.cpp
   vectormodel.cpp
   vectortablemodel.cpp
   viewcommand.cpp
   view.cpp
   viewitem.cpp
   )

kde4_add_ui_files(kstapp_LIB_SRCS
   datamanager.ui
   vectoreditordialog.ui
   )

kde4_automoc(${kstapp_LIB_SRCS})

kde4_add_library(kstapp SHARED ${kstapp_LIB_SRCS})

target_link_libraries(kstapp kstmath QtOpenGL)

set_target_properties(kstapp PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2)
install(TARGETS kstapp DESTINATION ${LIB_INSTALL_DIR} )
