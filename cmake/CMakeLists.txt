cmake_minimum_required(VERSION 2.8)

# figure out if we use MinGW with Ninja: call with -DCC=gcc
if(WIN32 AND CMAKE_GENERATOR MATCHES Ninja AND CC MATCHES gcc)
    find_file(mingw_make mingw32-make.exe)
    get_filename_component(mingw_bin_dir ${mingw_make} PATH)
    set(CMAKE_RC_COMPILER ${mingw_bin_dir}/windres.exe)
endif()

project(Kst)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
include(KstMacros)

get_filename_component(kst_dir ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)
get_filename_component(kst_build_dir ${CMAKE_BINARY_DIR}/build ABSOLUTE)

set(kst_binary_name kst2)


kst_option_init()
kst_option(version_string "Version string" OFF all)
kst_option(release "Build release version: optimize for speed, don't embedded debug symbols" OFF all)
kst_option(merge_files "Merge files to speedup build about factor 5" OFF all)
kst_option(merge_rebuild "Rebuild generated files from merged files build" OFF all)
kst_option(verbose "Make verbose CMake run and Makefiles" OFF all)
kst_option(install_prefix "Install path for Kst, using a default if not set" OFF all)
kst_option(install_libdir "Install directory name for libraries" OFF all)
kst_option(3rdparty "Build plugins depending on 3rd party libraries" ON all)
kst_option(dataobjects "Build dataobject plugins" ON all)
kst_option(test "Build unit tests" OFF all)
kst_option(pch "Use precompiled headers" ON all)
kst_option(svnversion "Use svnversion's output for Kst's version information" ON all)
kst_option(rpath "Use rpath" ON gcc)
kst_option(3rdparty_build "Download and build 3rd party libraries" OFF win)
kst_option(3rdparty_download "Download precompiled 3rd party libraries" OFF win)
kst_option(console "Open console on Windows" OFF win)
kst_option(edit_cont "Enable \"Edit and Continue\" for Visual Studio" OFF win)
kst_option(python "Build Python support" OFF all)

message(STATUS)


if(kst_release)
	set(CMAKE_BUILD_TYPE Release)
else()
	set(CMAKE_BUILD_TYPE Debug)
endif()


set(kst_version_major 2)
if(kst_version_string)
	set(kst_version ${kst_version_string})
else()
	set(kst_version_minor 0)
	set(kst_version_patch x)
	set(kst_version ${kst_version_major}.${kst_version_minor}.${kst_version_patch})
endif()

if(kst_3rdparty_build)
	set(kst_3rdparty_dir ${CMAKE_BINARY_DIR}/3rdparty/installed)
	add_subdirectory(3rdparty)
elseif(kst_3rdparty_download)
    if(MINGW)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
        message(STATUS "Using GCC version ${GCC_VERSION}")
        if(GCC_VERSION VERSION_LESS 4.6)
            set(ver 4.4)
            set(md5 999248fb40a44543af4dd4cd1be0ceeb)
        else(GCC_VERSION VERSION_GREATER 4.6)
            set(ver 4.6)
            set(md5 70d8670af9c21eb8fb466654c95d8e4d)
        else()
            message(FATA_ERROR "MinGW version ${GCC_VERSION} not supported")
        endif()
    endif()
	message(STATUS "Checking downloaded 3rd party binaries.")
	set(_deps kst-3rdparty-win32-gcc${ver})
	set(_file ${_deps}.zip)
	set(_downloaded _downloaded-NOTFOUND CACHE PATH "3rd party binaries already downloaded" FORCE)
	find_file(_downloaded ${_file} ${CMAKE_BINARY_DIR})
	if (NOT _downloaded)
		file(DOWNLOAD http://sourceforge.net/projects/kst/files/3rdparty/${_file}
            ${CMAKE_BINARY_DIR}/${_file}
			EXPECTED_MD5 ${md5}
			SHOW_PROGRESS)
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xfj ${CMAKE_BINARY_DIR}/${_file})
	endif()
	set(kst_3rdparty_dir ${CMAKE_BINARY_DIR}/${_deps})
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
	fix_it(QTCORE QTGUI QTXML QTSVG QTOPENGL QTNETWORK)
endif()

message(STATUS)
if(kst_3rdparty)
  message(STATUS "3rd party libs for plugins--------------------")
  find_package(Getdata)
  find_package(Gsl)
  find_package(Netcdf)
  find_package(Matio)
  find_package(CFITSIO)
  message(STATUS "----------------------------------------------")
else()
  message(STATUS "Building plugins depending on 3rd party libraries suppressed")
endif()
message(STATUS)


# React on options


if(kst_verbose)
	set(CMAKE_VERBOSE_MAKEFILE 1)
else()
	set(CMAKE_VERBOSE_MAKEFILE 0)
endif()

if(kst_install_prefix)
	set(CMAKE_INSTALL_PREFIX ${kst_install_prefix} CACHE PATH "User's choice for install prefix" FORCE)
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
else()
	set(kst_install_prefix INSTALLED)
	set(CMAKE_INSTALL_PREFIX ${kst_install_prefix} CACHE PATH "Kst's default install prefix" FORCE)
endif()

if(NOT kst_install_libdir)
	set(kst_install_libdir lib)
endif()

if(NOT kst_install_plugins)
	set(kst_install_plugins ${kst_install_libdir}/${kst_binary_name}/plugins)
endif()

if(kst_rpath)
	# the RPATH to be used when installing
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${kst_install_libdir}")
endif()

#path to install the .desktop files
set(kst_install_plugin_desktop_file_path share/services/kst)



if(NOT kst_console)
	set(kst_win32 WIN32)
	set(kst_qtmain_library ${QT_QTMAIN_LIBRARY})
endif()

if(APPLE AND NOT CMAKE_GENERATOR STREQUAL Xcode)
	set(kst_win32 MACOSX_BUNDLE)
endif()

if(APPLE)
	set(kst_pch 0 CACHE BOOL "Disable pch on Mac, not supported yet" FORCE)
endif()

if(kst_merge_files)
	set(kst_pch 0 CACHE BOOL "Disable pch because we build merged" FORCE)
endif()




if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES -DNOMINMAX)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if(MSVC_IDE AND kst_edit_cont)
	# Enable <Edit and Continue> in Visual Studio
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	if(kst_pch)
		# Studio bug: <Visual Studio 2005 "Force Includes" Breaks Edit and Continue with Pre-compiled Headers>
		set(kst_pch 0 CACHE BOOL "Disable pch because of Edit&Continue" FORCE)
	endif()
endif()



if(MSVC)
	set(kst_debug_postfix d)
endif()

configure_file(${kst_dir}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(${CMAKE_BINARY_DIR} ${QT_INCLUDES})

message(STATUS "Build summary----------------------------------")
message(STATUS)
message(STATUS "Version       : ${kst_version}")
message(STATUS "Path to source: ${kst_dir}")
message(STATUS "Installation  :")
message(STATUS "        ${kst_install_prefix}/bin")
message(STATUS "        ${kst_install_prefix}/${kst_install_libdir}")
message(STATUS "        ${kst_install_prefix}/${kst_install_plugins}")
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

if(kst_python)
    add_subdirectory(pyKst)
endif()

# CPack packaging
#
# http://cmake.org/cmake/help/cmake-2-8-docs.html#module:CPack

if(WIN32)
	set(CPACK_GENERATOR NSIS ZIP)
	set(CPACK_BINARY_ZIP 1)
endif()

if(kst_install_prefix)
	if(WIN32)
		set(CPACK_PACKAGE_FILE_NAME ${kst_install_prefix}-win32)
	else()
		set(CPACK_PACKAGE_FILE_NAME ${kst_install_prefix}-binary)
	endif()
	set(CPACK_SOURCE_PACKAGE_FILE_NAME ${kst_install_prefix})
else()
	set(CPACK_SOURCE_PACKAGE_FILE_NAME Kst-${kst_version})
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



# cmake bug, problems with the quotes in the gpl, fixed in 2.8.4
set(CPACK_RESOURCE_FILE_LICENSE "${kst_dir}/COPYING")



# sources
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${kst_dir};/") # http://www.mail-archive.com/cmake@cmake.org/msg33720.html
if(WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
else()
	set(CPACK_SOURCE_GENERATOR TGZ)
endif()

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
	# We start from build/bin/kst2.app, all is build into the final bundle folder
	# On the mac the pathes to needed shared libraries are hardcoded in the binary/library 
	# which could be changed by the macro 'fixup_bundle' 
	# libs which are hardcoded are resolved automatically
	# but the plugins must be passed as extra argument because they are not hardcoded in the binary 
	
	set(bundle_dir ${kst_build_dir}/bin)
	set(contents_dir ${kst_binary_name}.app/Contents)
	
	set(app ${bundle_dir}/${kst_binary_name}.app)
	install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${bundle_dir}/${contents_dir}/plugins COMPONENT Runtime)
	
	install(CODE 
		"file(GLOB qt_plugins \"${bundle_dir}/${contents_dir}/plugins/imageformats/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB kst_plugins \"${bundle_dir}/${contents_dir}/plugins/*${CMAKE_SHARED_MODULE_SUFFIX}\")
		set(libs \${qt_plugins} \${kst_plugins})
		include(BundleUtilities) 
		fixup_bundle(\"${app}\" \"\${libs}\" \"${dir}\") "
		COMPONENT RUNTIME)
	
	file(WRITE ${CMAKE_BINARY_DIR}/qt.conf "")
	install(FILES ${CMAKE_BINARY_DIR}/qt.conf DESTINATION ${contents_dir}/Resources COMPONENT Runtime)

	set(CPACK_GENERATOR DragNDrop)
endif()

include(CPack)

