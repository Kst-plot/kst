
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(fitstools_LIB_SRCS fitstools.cpp )

kde4_add_library(fitstools STATIC ${fitstools_LIB_SRCS})

target_link_libraries(fitstools  ${KDE4_KDECORE_LIBS} qt )

set_target_properties(fitstools PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2)
install(TARGETS fitstools DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

