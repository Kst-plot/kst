cmake_minimum_required(VERSION 2.8)

# figure out if we use MinGW with Ninja: call with -DCC=gcc
if(WIN32 AND CMAKE_GENERATOR MATCHES Ninja AND CC MATCHES gcc)
	find_file(mingw_make mingw32-make.exe)
	get_filename_component(mingw_bin_dir ${mingw_make} PATH)
	set(CMAKE_RC_COMPILER ${mingw_bin_dir}/windres.exe)
endif()


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")
include(KstMacros)

get_filename_component(kst_dir ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
get_filename_component(kst_build_dir ${CMAKE_BINARY_DIR}/build ABSOLUTE)

set(kst_binary_name kst2)


kst_option_init()
kst_option(string ""  all version_string    "Version string")
kst_option(bool   OFF all release           "Build release version: optimize for speed, don't embedded debug symbols")
kst_option(bool   OFF gcc deploy            "Deploy into install dir")
kst_option(bool   OFF all merge_files       "Merge files to speedup build about factor 5")
kst_option(bool   OFF all merge_rebuild     "Rebuild generated files from merged files build")
kst_option(bool   OFF all verbose           "Make verbose CMake run and Makefiles")
kst_option(string ""  all install_prefix    "Install path for Kst, using a default if not set")
kst_option(string ""  all install_libdir    "Install directory name for libraries")
kst_option(bool   ON  all 3rdparty          "Build plugins depending on 3rd party libraries")
kst_option(bool   ON  all dataobjects       "Build dataobject plugins")
kst_option(bool   OFF all test              "Build unit tests")
kst_option(bool   ON  all pch               "Use precompiled headers")
kst_option(bool   ON  all svnversion        "Use svnversion's output for Kst's version information")
kst_option(bool   OFF gcc rpath             "Use rpath")
kst_option(bool   OFF all 3rdparty_build    "Download and build 3rd party libraries")
kst_option(bool   OFF win 3rdparty_download "Download precompiled 3rd party libraries")
kst_option(bool   OFF win console           "Open console on Windows")
kst_option(bool   OFF win edit_cont         "Enable \"Edit and Continue\" for Visual Studio")
kst_option(bool   OFF all python            "Install Python support")
kst_option(bool   OFF all python_build      "Download and build NumPy/SciPy then install Python support")
kst_option(string ""  all python_prefix     "Path for installing python bindings")
kst_option(string OFF all cross             "Cross-compile on Linux for Windows")
kst_option(string ""  all qt4               "Use cross compiled Qt4 at given path")
kst_option(string ""  all qt5               "Use Qt5")
kst_option(bool   ON  all qt5base_only      "Only use Qt5 features available in qtbase")
kst_option(bool   ON  all opengl            "Use OpenGL")
kst_option(bool   OFF all clang             "Use Clang compiler")
kst_option(bool   OFF all sanitize          "Use Clang's sanitizers")

message(STATUS)

if(kst_cross)
    include(KstCrossCompile)
endif()


project(Kst)


if(kst_release)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
else()
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()


if(kst_sanitize)
    include(Clang-sanitize)
    set(clang_flags "${clang_flags}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${clang_flags}")
endif()


set(kst_version_major 2)
if(kst_version_string)
	set(kst_version ${kst_version_string})
else()
	set(kst_version_minor 0)
	set(kst_version_patch x)
	set(kst_version ${kst_version_major}.${kst_version_minor}.${kst_version_patch})
	set(kst_version_string ${kst_version})
endif()

if(MINGW)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	message(STATUS "Using GCC version ${GCC_VERSION}")
endif()

if(MINGW)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	message(STATUS "Using GCC version ${GCC_VERSION}")
endif()


if(kst_3rdparty_build)
	set(kst_3rdparty_dir ${CMAKE_BINARY_DIR}/3rdparty/installed)
	add_subdirectory(3rdparty)
elseif(kst_3rdparty_download)
	if(MINGW)
		if(GCC_VERSION VERSION_EQUAL 4.7.2 OR GCC_VERSION VERSION_GREATER 4.7.2)
			set(ver -dw2-4.7.2)
			set(md5 53edc4009d5c3ee5e11d1ba24cd8ae61)
		elseif(GCC_VERSION VERSION_EQUAL 4.7 OR GCC_VERSION VERSION_GREATER 4.7)
			set(ver 4.7)
			set(md5 de6e8dbab1bb17eee6057941fddc93e3)
		else(GCC_VERSION VERSION_EQUAL 4.6 OR GCC_VERSION VERSION_GREATER 4.6)
			set(ver 4.6)
			set(md5 70d8670af9c21eb8fb466654c95d8e4d)
		else(GCC_VERSION VERSION_GREATER 4.4)
			set(ver 4.4)
			set(md5 999248fb40a44543af4dd4cd1be0ceeb)
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
if(kst_revision)
	file(WRITE ${CMAKE_BINARY_DIR}/svnrevision.h.tmp "#define SVN_REVISION \"${kst_revision}\"\n")
	configure_file(${CMAKE_BINARY_DIR}/svnrevision.h.tmp ${CMAKE_BINARY_DIR}/svnrevision.h COPYONLY)
elseif(kst_svnversion)
	include(SubversionHeader)
	SubversionHeader(${kst_dir} ${kst_revision_project} ${CMAKE_BINARY_DIR}/svnrevision.h _modified)
else()
	file(WRITE ${CMAKE_BINARY_DIR}/svnrevision.h.tmp "#define SVN_REVISION \"unknown\"\n")
	configure_file(${CMAKE_BINARY_DIR}/svnrevision.h.tmp ${CMAKE_BINARY_DIR}/svnrevision.h COPYONLY)
endif()


# Find 3rd party libraries
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



# Find 3rd party libraries
if (kst_qt5)
	# http://doc-snapshot.qt-project.org/5.0/cmake-manual.html
	cmake_minimum_required(VERSION 2.8.9)
	macro(use_qt5lib qt5lib)
		find_package(${qt5lib} REQUIRED)
		include_directories(${${qt5lib}_INCLUDE_DIRS})
	endmacro()
	use_qt5lib(Qt5Core)
	get_target_property(qmakebin Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(QT_BINARY_DIR ${qmakebin} PATH)
	message(STATUS "Using Qt5 from ${QT_BINARY_DIR}")
	set(CMAKE_PREFIX_PATH ${QT_BINARY_DIR}/..)
	use_qt5lib(Qt5Concurrent)
	use_qt5lib(Qt5Widgets)
	use_qt5lib(Qt5Network)
	use_qt5lib(Qt5Xml)
	use_qt5lib(Qt5PrintSupport)
	if(kst_opengl)
	use_qt5lib(Qt5OpenGL)
	else()
		add_definitions(-DKST_NO_OPENGL)
	endif()
	add_definitions(-DQT5=1 -DQT_DISABLE_DEPRECATED_BEFORE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")
	include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS}) # Qt5 bug?
else()
	if(kst_cross)
		# Cross-compiled Qt, branch 4.8.4 from https://gitorious.org/~syntheticpp/qt/qt4
		set(QT_MINGW_DIR ${kst_qt4} CACHE PATH "Qt for Mingw" FORCE)
		set(CMAKE_FIND_ROOT_PATH /usr/${TOOLNAME} ${QT_MINGW_DIR} CACHE PATH "Mingw find root path" FORCE)
	endif()
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
	set(kst_install_prefix ${CMAKE_BINARY_DIR}/INSTALLED)
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
	if(kst_qt5 AND WIN32)
		set(kst_qtmain_library Qt5::WinMain)
	else()
		set(kst_qtmain_library ${QT_QTMAIN_LIBRARY})
	endif()
endif()

if(APPLE AND NOT CMAKE_GENERATOR STREQUAL Xcode)
	set(kst_win32 MACOSX_BUNDLE)
endif()

if(APPLE OR kst_clang)
	set(kst_pch 0 CACHE BOOL "Disable pch on Mac, not supported yet" FORCE)
endif()

if(kst_merge_files)
	set(kst_pch 0 CACHE BOOL "Disable pch because we build merged" FORCE)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(x64 TRUE)
endif()


if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501) # Windows XP
	# on 32 bit Windows we must explicitely enable >2GB support
	# maybe you have to ' 4-gigabyte tune' your Windows XP 32 bit system:
	# http://msdn.microsoft.com/en-us/library/windows/desktop/aa366778%28v=vs.85%29.aspx
	macro(large_address var flag)
		set(${var} "${${var}} ${flag}")
	endmacro()
	macro(link_large_address flag)
		large_address(CMAKE_EXE_LINKER_FLAGS    ${flag})
		large_address(CMAKE_SHARED_LINKER_FLAGS ${flag})
		large_address(CMAKE_MODULE_LINKER_FLAGS ${flag})
	endmacro()
	if(MSVC)
		link_large_address(/LARGEADDRESSAWARE)
	elseif(NOT x64)
		link_large_address(-Wl,--large-address-aware)
	endif()
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
message(STATUS "Used options:")
kst_option_list_all()
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
	if(kst_cross)
		set(CPACK_GENERATOR ZIP)
	else()
		set(CPACK_GENERATOR ZIP NSIS)
	endif()
	set(CPACK_BINARY_ZIP 1)
endif()


if(kst_install_prefix)
	if(WIN32)
		if(x64)
			set(CPACK_PACKAGE_FILE_NAME ${kst_install_prefix}-win64)
		else()
			set(CPACK_PACKAGE_FILE_NAME ${kst_install_prefix}-win32)
		endif()
	else()
		set(CPACK_PACKAGE_FILE_NAME kst-${kst_version_string}-binary)
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


