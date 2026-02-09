# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

include(GenerateExportHeader)

add_library(Kst6Widgets SHARED)

generate_export_header(Kst6Widgets
    BASE_NAME KSTWIDGETS
)

target_include_directories(Kst6Widgets
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Kst6Widgets_autogen/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Kst6Widgets>"
)

target_sources(Kst6Widgets PRIVATE
    cclineedit.h
    cclineedit_p.h
    colorbutton.h
    colorpalette.h
    combobox.h
    curveappearance.h
    curveplacement.h
    curveselector.h
    datarange.h
    datasourceselector.h
    datasourceselectordialog.h
    dialogdefaults.h
    fftoptions.h
    filerequester.h
    geticon.h
    gradienteditor.h
    kstwidgets_export.h
    labelbuilder.h
    labellineedit.h
    matrixselector.h
    modelessinfobox.h
    scalarlistselector.h
    scalarselector.h
    stringselector.h
    vectorselector.h
    cclineedit.cpp
    colorbutton.cpp
    colorpalette.cpp
    combobox.cpp
    curveappearance.cpp
    curveplacement.cpp
    curveselector.cpp
    datarange.cpp
    datasourceselector.cpp
    datasourceselectordialog.cpp
    dialogdefaults.cpp
    fftoptions.cpp
    filerequester.cpp
    geticon.cpp
    gradienteditor.cpp
    labelbuilder.cpp
    labellineedit.cpp
    matrixselector.cpp
    modelessinfobox.cpp
    scalarlistselector.cpp
    scalarselector.cpp
    stringselector.cpp
    vectorselector.cpp
    colorpalette.ui
    curveappearance.ui
    curveplacement.ui
    curveselector.ui
    datarange.ui
    fftoptions.ui
    labelbuilder.ui
    matrixselector.ui
    modelessinfobox.ui
    scalarlistselector.ui
    scalarselector.ui
    stringselector.ui
    vectorselector.ui
)

target_link_libraries(Kst6Widgets PUBLIC
    Qt6::Widgets
    Qt6::Xml
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Svg
    Qt6::Designer
    Kst6Core
    Kst6Math
)

install(FILES
    ${KSTWIDGETS_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kstwidgets_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Kst6Widgets
)

set_target_properties(Kst6Widgets PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/kst"
)

install(TARGETS Kst6Widgets
    EXPORT KstTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/kst
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/kst
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
