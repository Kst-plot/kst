
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstmath_LIB_SRCS 
   kstpainter.cpp 
   kstcolorsequence.cpp 
   kstsettings.cpp 
   kstobjectdefaults.cpp 
   labelparser.cpp 
   kstdataobject.cpp 
   kstdataobjectcollection.cpp 
   kstcsd.cpp 
   kstpsd.cpp 
   kstbasecurve.cpp 
   kstcurvepointsymbol.cpp 
   kstlinestyle.cpp 
   kstvcurve.cpp 
   kstimage.cpp 
   kstdefaultnames.cpp 
   ksthistogram.cpp 
   kstequation.cpp 
   kstcurvehint.cpp 
   kstbasicplugin.cpp 
   fftsg_h.c 
   enodes.cpp 
   enodefactory.cpp 
   dialoglauncher.cpp 
   eparse-eh.cpp 
   eparse.c 
   psdcalculator.cpp 
   escan.c
   objectfactory.cpp
   relationfactory.cpp
   builtinobjects.cpp
   )

kde4_automoc(${kstmath_LIB_SRCS})

kde4_add_library(kstmath SHARED ${kstmath_LIB_SRCS})

target_link_libraries(kstmath  ${KDE4_KDECORE_LIBS} kst )

set_target_properties(kstmath PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2 )
install(TARGETS kstmath DESTINATION ${LIB_INSTALL_DIR} )

