
#NOTE this is used to fill in the latest revision number in kstrevision.h
# find_package(Subversion)
# if(Subversion_FOUND)
#   Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} libkst)
# endif(Subversion_FOUND)

#FIXME need to run this whenever .svn/entries changes...
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kstrevision.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kstrevision.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ksttimers.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ksttimers.h)

include_directories(${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES})

########### next target ###############

set(kst_LIB_SRCS 
   kstobject.cpp 
   kstdatasource.cpp 
   stdinsource.cpp 
   kstextension.cpp 
   kstdatacollection.cpp 
   kstdebug.cpp 
   rwlock.cpp 
   kstmath.cpp 
   kstdateparser.cpp 
   ksttimezones.cpp 
   kstscalar.cpp 
#   $(PROCPS_COPY) 
   kststring.cpp 
   kstmatrix.cpp 
   kstrmatrix.cpp 
   kstsmatrix.cpp 
   kstamatrix.cpp 
   kstvector.cpp 
   kstsvector.cpp 
   kstavector.cpp 
   kstrvector.cpp 
   kstvectordefaults.cpp 
   kstmatrixdefaults.cpp 
   kstprimitive.cpp 
   defaultprimitivenames.cpp)

kde4_automoc(${kst_LIB_SRCS})

kde4_add_library(kst SHARED ${kst_LIB_SRCS})

# target_link_libraries(kst kstextdate)
target_link_libraries(kst ${KDE4_KDECORE_LIBS} ${KDE4_KDE3SUPPORT_LIBS})

set_target_properties(kst PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 2)
install(TARGETS kst DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install(FILES kstdatasource.h kstobject.h kst_export.h kstsharedptr.h rwlock.h kstdateparser.h kstobjectcollection.h DESTINATION ${INCLUDE_INSTALL_DIR})



