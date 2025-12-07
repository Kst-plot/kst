cmake_minimum_required(VERSION 3.22)

set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.5.0")

set(kst_version_major 3)
set(kst_version_minor 0)
set(kst_version_patch 0)
set(kst_version ${kst_version_major}.${kst_version_minor}.${kst_version_patch})
set(kst_version_string ${kst_version})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Kst VERSION ${kst_version})

include(FeatureSummary)

find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    URL "https://api.kde.org/ecm/"
    DESCRIPTION "extra cmake modules")

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

include(KstMacros)

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMEnableSanitizers)

get_filename_component(kst_dir ${CMAKE_SOURCE_DIR} ABSOLUTE)
get_filename_component(kst_build_dir ${CMAKE_BINARY_DIR}/build ABSOLUTE)

set(kst_binary_name kst2)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX KST
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kst_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KstConfigVersion.cmake"
    SOVERSION "3"
)

kst_option_init()
kst_option(bool   OFF all merge_files       "Merge files to speedup build about factor 5")
kst_option(bool   OFF all merge_rebuild     "Rebuild generated files from merged files build")
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
kst_option(bool   OFF all python_depend     "Download and build NumPy/SciPy then install Python support")
kst_option(string ""  all python_prefix     "Path for installing python bindings")
kst_option(bool   OFF  all qt6base_only      "Only use Qt5 features available in qtbase")
kst_option(bool   OFF all dbgsym            "Build with debug symbols enabled")
kst_option(bool   OFF win noinstaller       "Don't build installer")

message(STATUS)

if(kst_3rdparty_build)
    set(kst_3rdparty_dir ${CMAKE_BINARY_DIR}/3rdparty/installed)
    add_subdirectory(cmake/3rdparty)
elseif(kst_3rdparty_download)
    if(MINGW)
        if(GCC_VERSION VERSION_EQUAL 4.7.2 OR GCC_VERSION VERSION_GREATER 4.7.2)
            set(ver -dw2-4.7.2)
            set(md5 53edc4009d5c3ee5e11d1ba24cd8ae61)
        elseif(GCC_VERSION VERSION_EQUAL 4.7 OR GCC_VERSION VERSION_GREATER 4.7)
            set(ver 4.7)
            set(md5 de6e8dbab1bb17eee6057941fddc93e3)
        elseif(GCC_VERSION VERSION_EQUAL 4.6 OR GCC_VERSION VERSION_GREATER 4.6)
            set(ver 4.6)
            set(md5 70d8670af9c21eb8fb466654c95d8e4d)
        elseif(GCC_VERSION VERSION_GREATER 4.4)
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


# Find 3rd party libraries
if(kst_3rdparty)
  message(STATUS "3rd party libs for plugins--------------------")
  find_package(Getdata)
  find_package(GSL)
  find_package(Netcdf)
  find_package(Matio)
  find_package(CFITSIO)
  find_package(TIFF)
  find_package(HDF5)
  message(STATUS "----------------------------------------------")
else()
  message(STATUS "Building plugins depending on 3rd party libraries suppressed")
endif()
message(STATUS)

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS
    Core
    Concurrent
    Widgets
    Network
    Xml
    PrintSupport
    Svg
    LinguistTools
    Designer
)
set_package_properties(Qt6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS I18n CoreAddons)
set_package_properties(KF6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")

# React on options

if(NOT kst_console)
    set(kst_win32 WIN32)
    if(WIN32)
        set(kst_qtmain_library Qt6::WinMain)
    endif()
endif()

if(APPLE AND NOT CMAKE_GENERATOR STREQUAL Xcode)
    set(kst_win32 MACOSX_BUNDLE)
endif()

if(APPLE OR kst_clang)
    set(kst_pch 0 CACHE BOOL "Disable pch on Mac, not supported yet" FORCE)
endif()

if (APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()

if(kst_merge_files)
    set(kst_pch 0 CACHE BOOL "Disable pch because we build merged" FORCE)
endif()

if(MSVC)
    add_definitions(-D_USE_MATH_DEFINES -DNOMINMAX)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    if(kst_dbgsym)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()
endif()

if(MSVC_IDE AND kst_edit_cont)
    # Enable <Edit and Continue> in Visual Studio
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    if(kst_pch)
        # Studio bug: <Visual Studio 2005 "Force Includes" Breaks Edit and Continue with Pre-compiled Headers>
        set(kst_pch 0 CACHE BOOL "Disable pch because of Edit&Continue" FORCE)
    endif()
endif()

configure_file(${kst_dir}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if (NOT APPLE)
    add_subdirectory(misc)
endif()

if(kst_python)
    add_subdirectory(pyKst)
endif()

#adapted from [trojita.git] / CMakeLists.txt
if(Qt6LinguistForKst_FOUND)
    file(GLOB_RECURSE kst_PO "${CMAKE_SOURCE_DIR}/po/kst_common_*.po")
    qt6_wrap_po(kst_QM ${kst_PO})
    set(language_summary "")
    foreach(po ${kst_PO})
        string(REGEX REPLACE "^(.*)/kst_common_(.*).po" "\\2" lang ${po})
        list(APPEND language_summary ${lang})
    endforeach()
    list(SORT language_summary)
    list(LENGTH language_summary num_languages)
    if(num_languages)
        message(STATUS "Available languages: ${language_summary}")
        add_custom_target(qm DEPENDS ${kst_QM})
        add_dependencies(${kst_binary_name} qm)
        install(DIRECTORY ${CMAKE_BINARY_DIR}/locale/ DESTINATION share/kst/locale FILES_MATCHING PATTERN *.qm)
    else()
        message(STATUS "No .po files found, will not install any languages")
    endif()
else()
    message(STATUS "Qt Linguist (lupdate/lrelease/lconvert) not found, disabling localization support")
endif()
