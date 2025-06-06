# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Kst6Widgets SHARED)

ecm_generate_export_header(Kst6Widgets
    BASE_NAME KSTWIDGETS
    VERSION ${PROJECT_VERSION}
    USE_VERSION_HEADER kst_version.h
)

target_include_directories(Kst6Widgets
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Kst6Widgets}>"
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
    widgets.h
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
    widgets.cpp
    colorpalette.ui
    curveappearance.ui
    curveplacement.ui
    curveselector.ui
    datarange.ui
    fftoptions.ui
    kstwidgets_pch
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
    KF6::I18n
    Kst6Core
    Kst6Math
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kstwidgets_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Kst6Widgets
    COMPONENT Devel
)

install(TARGETS Kst6Widgets EXPORT KstTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
