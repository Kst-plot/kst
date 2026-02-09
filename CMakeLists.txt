cmake_minimum_required(VERSION 3.22)

set(QT_MIN_VERSION "6.4.0")

set(kst_version_major 3)
set(kst_version_minor 0)
set(kst_version_patch 0)
set(kst_version ${kst_version_major}.${kst_version_minor}.${kst_version_patch})
set(kst_version_string ${kst_version})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Kst VERSION ${kst_version})

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Ensure custom CMake modules (e.g., FindGetdata.cmake) are found
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

include(KstMacros)

include(FeatureSummary)

get_filename_component(kst_dir ${CMAKE_SOURCE_DIR} ABSOLUTE)
get_filename_component(kst_build_dir ${CMAKE_BINARY_DIR} ABSOLUTE)

set(kst_binary_name kst)

set(kst_install_plugins "${CMAKE_INSTALL_LIBDIR}/kst" CACHE PATH "Kst plugin installation directory")

# Add more options as needed, or revisit legacy options if required

message(STATUS)



# Find 3rd party libraries for plugins
message(STATUS "3rd party libs for plugins--------------------")
find_package(Getdata)
find_package(GSL)
find_package(Netcdf)
find_package(Matio)
find_package(CFITSIO)
find_package(TIFF)
find_package(HDF5)
message(STATUS "----------------------------------------------")

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


feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")


configure_file(${kst_dir}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(${CMAKE_BINARY_DIR})


add_subdirectory(src)

if (NOT APPLE)
    add_subdirectory(misc)
endif()


#adapted from [trojita.git] / CMakeLists.txt
if(Qt6LinguistTools_FOUND)
    file(GLOB_RECURSE kst_PO "${CMAKE_SOURCE_DIR}/po/kst_common_*.po")
    set(language_summary "")
    foreach(po ${kst_PO})
        string(REGEX REPLACE "^(.*)/kst_common_(.*).po" "\\2" lang ${po})
        list(APPEND language_summary ${lang})
    endforeach()
    list(SORT language_summary)
    list(LENGTH language_summary num_languages)
    if(num_languages)
        message(STATUS "Available languages: ${language_summary}")
        qt_add_translations(kst PO_FILES ${kst_PO})
        install(DIRECTORY ${CMAKE_BINARY_DIR}/locale/ DESTINATION ${CMAKE_INSTALL_LOCALEDIR} FILES_MATCHING PATTERN *.qm)
    else()
        message(STATUS "No .po files found, will not install any languages")
    endif()
else()
    message(STATUS "Qt Linguist (lupdate/lrelease/lconvert) not found, disabling localization support")
endif()
