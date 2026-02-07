# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB MOC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
qt6_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

include(GenerateExportHeader)

add_library(Kst6Core SHARED)

generate_export_header(Kst6Core
    BASE_NAME KSTCORE
)

target_include_directories(Kst6Core
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Kst6}>"
)

target_sources(Kst6Core PRIVATE
    ${MOC_SOURCES}
    builtinprimitives.h
    coredocument.h
    datacollection.h
    datamatrix.h
    dataplugin.h
    dataprimitive.h
    datascalar.h
    datasource.h
    datasourcefactory.h
    datasourcepluginmanager.h
    datastring.h
    datavector.h
    dateparser.h
    debug.h
    editablematrix.h
    editablevector.h
    events.h
    extension.h
    generatedmatrix.h
    generatedvector.h
    index_kst.h
    kst_inf.h
    ksttimers.h
    ksttimezone.h
    labelinfo.h
    logevents.h
    math_kst.h
    matrix.h
    matrixfactory.h
    matrixscriptinterface.h
    measuretime.h
    namedobject.h
    nextcolor.h
    object.h
    objectlist.h
    objectmap.h
    objectstore.h
    plotiteminterface.h
    primitive.h
    primitivefactory.h
    procps.h
    psversion.h
    rwlock.h
    scalar.h
    scalarfactory.h
    scalarscriptinterface.h
    scriptinterface.h
    settings.h
    string_kst.h
    stringfactory.h
    stringscriptinterface.h
    sysinfo.h
    updatemanager.h
    updateserver.h
    vector.h
    vectorfactory.h
    vectorscriptinterface.h
    vscalar.h
    psversion.c
    sysinfo.c
    builtinprimitives.cpp
    coredocument.cpp
    datacollection.cpp
    datamatrix.cpp
    dataprimitive.cpp
    datascalar.cpp
    datasource.cpp
    datasourcefactory.cpp
    datasourcepluginmanager.cpp
    datastring.cpp
    datavector.cpp
    dateparser.cpp
    debug.cpp
    editablematrix.cpp
    editablevector.cpp
    extension.cpp
    generatedmatrix.cpp
    generatedvector.cpp
    ksttimezone.cpp
    labelinfo.cpp
    math_kst.cpp
    matrix.cpp
    matrixfactory.cpp
    matrixscriptinterface.cpp
    measuretime.cpp
    namedobject.cpp
    nextcolor.cpp
    object.cpp
    objectlist.cpp
    objectmap.cpp
    objectstore.cpp
    plotiteminterface.cpp
    primitive.cpp
    primitivefactory.cpp
    rwlock.cpp
    scalar.cpp
    scalarfactory.cpp
    scalarscriptinterface.cpp
    scriptinterface.cpp
    settings.cpp
    shortnameindex.cpp
    string_kst.cpp
    stringfactory.cpp
    stringscriptinterface.cpp
    updatemanager.cpp
    updateserver.cpp
    vector.cpp
    vectorfactory.cpp
    vectorscriptinterface.cpp
    vscalar.cpp
)

target_link_libraries(Kst6Core PUBLIC
    Qt6::Widgets
    Qt6::Xml
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Svg
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kstcore_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Kst6Core
)

install(TARGETS Kst6Core
    EXPORT KstTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
