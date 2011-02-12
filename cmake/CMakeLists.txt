cmake_minimum_required(VERSION 2.8)

project(Kst)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
include(KstMacros)

get_filename_component(kst_dir ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)


set(kst_version_major 2)
set(kst_version_minor 0)
set(kst_version_patch 3)
set(kst_version ${kst_version_major}.${kst_version_minor}.${kst_version_patch})

set(kst_binary_name kst2)


kst_option_init()
kst_option(release "Build release version" OFF all)
kst_option(merge_files "Merge files to speedup build about factor 5" OFF all)
kst_option(merge_rebuild "Rebuild generated files from merged files build" OFF all)
kst_option(verbose "Make verbose CMake run and Makefiles" OFF all)
kst_option(install_prefix "Install path for Kst, using a default if not set" OFF all)
kst_option(console "Open console on Windows" OFF win)
kst_option(3rdparty "Build plugins depending on 3rd party libraries" ON all)
kst_option(3rdparty_download "Download precompiled 3rd party libraries" OFF win)
kst_option(dataobjects "Build dataobject plugins" ON all)
kst_option(test "Build unit tests" OFF all)
kst_option(pch "Use precompiled headers" ON all)
kst_option(edit_cont "Enable \"Edit and Continue\" for Visual Studio" OFF win)
kst_option(svnversion "Use svnversion's output for Kst's version information" ON all)

message(STATUS)


if(kst_3rdparty_download)
	message(STATUS "Checking downloaded 3rd party binaries.")
	set(_deps kst-3rdparty-win32-deps)
	set(_file ${_deps}.tar.bz2)
	set(_downloaded _downloaded-NOTFOUND CACHE PATH "3rd party binaries already downloaded" FORCE)
	find_file(_downloaded ${_file} ${CMAKE_BINARY_DIR})
	if (NOT _downloaded)
		# TODO move to sf.net, but sf.net is broken atm (Jan 30 2011)
		file(DOWNLOAD http://kst-plot.kde.org/download/${_file} ${CMAKE_BINARY_DIR}/${_file}
			EXPECTED_MD5 d2ffdb6dad509fe40f1cc9f60c88ddd5
			SHOW_PROGRESS)
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xfj ${CMAKE_BINARY_DIR}/${_file})
	endif()
	set(kst_3rdparty_dir ${CMAKE_BINARY_DIR}/${_deps}-1.0)
endif()


kst_revision_project_name(Revision)
add_definitions(-DKST_HAVE_SVN_REVISION_H)
if(kst_svnversion)
	include(SubversionHeader)
	SubversionHeader(${kst_dir} ${kst_revision_project} ${CMAKE_BINARY_DIR}/svnrevision.h _modified)
else()
	file(WRITE ${CMAKE_BINARY_DIR}/svnrevision.h.tmp "#define SVN_REVISION \"unknown\"\n")
	configure_file(${CMAKE_BINARY_DIR}/svnrevision.h.tmp ${CMAKE_BINARY_DIR}/svnrevision.h COPYONLY)
endif()

# Find 3rd party libraries
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
if(MINGW)
	# seems there is a bug in FindQt4
	macro(fix_it)
		foreach(_it ${ARGN})
			if(QT_${_it}_LIBRARY_DEBUG AND QT_${_it}_LIBRARY_RELEASE)
				set(QT_${_it}_LIBRARY debug ${QT_${_it}_LIBRARY_DEBUG} optimized ${QT_${_it}_LIBRARY_RELEASE} CACHE STRING "Fixed ${_it}" FORCE)
			endif()
		endforeach()
	endmacro()
	fix_it(QTCORE QTGUI QTXML QTSVG QTOPENGL)
endif()

message(STATUS)
if(kst_3rdparty)
  message(STATUS "3rd party libs for plugins--------------------")
  find_package(Getdata)
  find_package(Gsl)
  find_package(Netcdf)
  find_package(CFITSIO)
  #find_package(Dmc)
  message(STATUS "----------------------------------------------")
else()
  message(STATUS "Building plugins depending on 3rd party libraries suppressed")
endif()
message(STATUS)


# React on options

if(kst_release)
	set(CMAKE_BUILD_TYPE Release)
else()
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(kst_verbose)
	set(CMAKE_VERBOSE_MAKEFILE 1)
else()
	set(CMAKE_VERBOSE_MAKEFILE 0)
endif()

if(kst_install_prefix)
	set(CMAKE_INSTALL_PREFIX ${kst_install_prefix} CACHE PATH "User's choice for install prefix" FORCE)
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX INSTALLED CACHE PATH "Kst's default install prefix" FORCE)
endif()

if(NOT kst_console)
	set(kst_win32 WIN32)
	set(kst_qtmain_library ${QT_QTMAIN_LIBRARY})
endif()

if(APPLE)
	set(kst_win32 MACOSX_BUNDLE)
endif()

if(APPLE)
	set(kst_pch 0 CACHE BOOL "Disable pch on Mac, not supported yet" FORCE)
endif()

if(kst_merge_files)
	set(kst_pch 0 CACHE BOOL "Disable pch because we build merged" FORCE)
endif()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

#path to install the .desktop files
set(kst_install_plugin_desktop_file_path share/services/kst)


if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES)
endif()

if(MSVC_IDE AND kst_edit_cont)
	# Enable <Edit and Continue> in Visual Studio
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	if(kst_pch)
		# Studio bug: <Visual Studio 2005 "Force Includes" Breaks Edit and Continue with Pre-compiled Headers>
		set(kst_pch 0 CACHE BOOL "Disable pch because of Edit&Continue" FORCE)
	endif()
endif()



if(WIN32)
	set(kst_debug_postfix d)
endif()

configure_file(${kst_dir}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(${CMAKE_BINARY_DIR} ${QT_INCLUDES})

message(STATUS "Build summary----------------------------------")
message(STATUS)
message(STATUS "Version          : ${kst_version}")
message(STATUS "Path to source   : ${kst_dir}")
message(STATUS "Installation path: ${CMAKE_INSTALL_PREFIX}")
message(STATUS)
message(STATUS "Options:")
kst_option_list_all()
message(STATUS)
message(STATUS "-----------------------------------------------")
message(STATUS)


add_subdirectory(src)

if(kst_test)
	enable_testing()
	add_subdirectory(tests)
endif()

if (NOT APPLE)
	add_subdirectory(misc)
endif()



# CPack packaging
#
# http://cmake.org/cmake/help/cmake-2-8-docs.html#module:CPack

if(WIN32)
	set(CPACK_GENERATOR NSIS ZIP)
	set(CPACK_BINARY_ZIP 1)
	if (kst_install_prefix)
		set(CPACK_NSIS_PACKAGE_NAME ${kst_install_prefix})
	endif()
endif()

if (kst_install_prefix)
	set(CPACK_PACKAGE_FILE_NAME ${kst_install_prefix})
	set(CPACK_SOURCE_PACKAGE_FILE_NAME ${kst_install_prefix}-sources)
else()
	set(CPACK_SOURCE_PACKAGE_FILE_NAME Kst-${kst_version}-sources)
endif()
set(CPACK_INSTALL_PREFIX "")
	

set(CPACK_PACKAGE_NAME   Kst)
set(CPACK_PACKAGE_EXECUTABLES ${kst_binary_name} Kst2)
set(CPACK_PACKAGE_VENDOR "The Kst Team")
set(CPACK_PACKAGE_VERSION_MAJOR ${kst_version_major})
set(CPACK_PACKAGE_VERSION_MINOR ${kst_version_minor})
set(CPACK_PACKAGE_VERSION_PATCH ${kst_version_patch})


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Kst 2")
set(CPACK_PACKAGE_DESCRIPTION_FILE    ${kst_dir}/README)
set(CPACK_PACKAGE_CONTACT "http://bugs.kde.org}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Kst ${kst_version}")
set(CPACK_STRIP_FILES 1)


if(UNIX)
	# cmake bug, problems with the quotes in the gpl, fixed in 2.8.4
	set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_BINARY_DIR}/gpl)
	FILE(WRITE ${CPACK_RESOURCE_FILE_LICENSE} "\n\nGNU GENERAL PUBLIC LICENSE, Version 2, June 1991")
else()
	set(CPACK_RESOURCE_FILE_LICENSE "${kst_dir}/COPYING")
endif()


# sources
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${kst_dir};/") # http://www.mail-archive.com/cmake@cmake.org/msg33720.html
set(CPACK_SOURCE_GENERATOR TGZ ZIP TBZ2)

# rpm
set(CPACK_SET_DESTDIR "ON")
FILE(READ ${CPACK_RESOURCE_FILE_LICENSE} CPACK_RPM_PACKAGE_LICENSE)
set(CPACK_RPM_PACKAGE_GROUP "Applications/Science")
set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})

# deb
# This is experimental
# check also for plugin's 3rdparty libraries?
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtgui4 (>= 4.4.3-0)")




# Usage
#
# Source only .targz, .tar.bz2, .zip:
#   make package_source
#
# Binary .tar.gz and install .sh:
#   make package
#
# Binary .deb:
# create : cpack -G DEB --config CPackConfig.cmake
# list   : dpkg-deb -c Kst-*.deb
# install: dpkg -i Kst-*.deb
#
# Binary .rpm:
# create : cpack -G RPM --config CPackConfig.cmake
# list   : rpm -qlp Kst-*.rpm              
# install: rpm -U Kst-*.rpm                


# CPack and Ubuntu
# Found this:
# http://www.purplekarrot.net/blog/dputCMake.html


if(APPLE)
	# We start from bin/kst2.app, all is build into the final bundle folder
	# On the mac the pathes to needed shared libraries are hardcoded in the binary/library 
	# which could be changed by the macro 'fixup_bundle' 
	# libs which are hardcoded are resolved automatically
	# but the plugins must be passed as extra argument because they are not hardcoded in the binary 

	set(app ${CMAKE_BINARY_DIR}/bin/${kst_binary_name}.app)
	set(contents_dir ${kst_binary_name}.app/Contents)
	#TODO copy not intsall! install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION  ${contents_dir}/plugins COMPONENT Runtime)
	
	file(WRITE ${CMAKE_BINARY_DIR}/qt.conf "")
	install(FILES ${CMAKE_BINARY_DIR}/qt.conf DESTINATION ${contents_dir}/Resources COMPONENT Runtime)
	
	install(CODE 
		"file(GLOB qt_plugins \"\${CMAKE_INSTALL_PREFIX}/${contents_dir}/plugins/imageformats/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB kst_plugins \"${CMAKE_BINARY_DIR}/bin/${contents_dir}/plugins/*${CMAKE_SHARED_MODULE_SUFFIX}\")
		set(libs \${qt_plugins_disbaled} \${kst_plugins})
		include(BundleUtilities) 
		fixup_bundle(\"${app}\" \"\${libs}\" \"${dir}\") "
		COMPONENT RUNTIME)
		
	set(CPACK_GENERATOR DragNDrop)
endif()

include(CPack)

