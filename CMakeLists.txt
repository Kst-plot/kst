project(kst)

# Path for cmake modules before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/kst/cmake/modules")

set(KST_VERSION "1.9.0_devel")
set(KST_LIBKST_VERSION 1.9.0)

find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1 -DQT_THREAD_SUPPORT=1)

link_directories(${KDE4_LIB_DIR})

#include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${QT_INCLUDES})

# create config.h
include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(misc)

########### install files ###############

#EXTRA_DIST = AUTHORS COPYING COPYING.LGPL ChangeLog INSTALL README TODO RELEASE.NOTES kst.lsm 
#
#changes:
#	(cd $(srcdir) && svn log --xml | xsltproc svn2cl.xsl - > ChangeLog)
#
#svnchanges: changes
#	(cd $(srcdir) && svn commit -m 'update changelog' ChangeLog)
#
