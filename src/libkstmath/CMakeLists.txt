# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(Kst6Math SHARED)

ecm_generate_export_header(Kst6Math
    BASE_NAME KSTMATH
    VERSION ${PROJECT_VERSION}
    USE_VERSION_HEADER kst_version.h
)

target_include_directories(Kst6Math
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KstMath6}>"
)

target_sources(Kst6Math PRIVATE
    basicplugin.h
    basicpluginfactory.h
    builtinobjects.h
    builtinpalettes.h
    builtinrelations.h
    colorsequence.h
    csd.h
    csdfactory.h
    curve.h
    curvefactory.h
    curvehint.h
    curvepointsymbol.h
    dataobject.h
    dataobjectplugin.h
    dataobjectscriptinterface.h
    dialoglauncher.h
    doublecompare.h
    enodefactory.h
    enodes.h
    eparse-eh.h
    eparse.h
    equation.h
    equationfactory.h
    eventmonitorentry.h
    eventmonitorfactory.h
    histogram.h
    histogramfactory.h
    image.h
    imagefactory.h
    kstmath_export.h
    labelparser.h
    linestyle.h
    objectfactory.h
    painter.h
    palette.h
    plotdefines.h
    plottickcalculator.h
    psd.h
    psdcalculator.h
    psdfactory.h
    relation.h
    relationfactory.h
    relationscriptinterface.h
    fftsg_h.c
    basicplugin.cpp
    basicpluginfactory.cpp
    builtinobjects.cpp
    builtinrelations.cpp
    colorsequence.cpp
    csd.cpp
    csdfactory.cpp
    curve.cpp
    curvefactory.cpp
    curvehint.cpp
    curvepointsymbol.cpp
    dataobject.cpp
    dataobjectscriptinterface.cpp
    dialoglauncher.cpp
    enodefactory.cpp
    enodes.cpp
    eparse-eh.cpp
    eparse.cpp
    equation.cpp
    equationfactory.cpp
    escan.cpp
    eventmonitorentry.cpp
    eventmonitorfactory.cpp
    histogram.cpp
    histogramfactory.cpp
    image.cpp
    imagefactory.cpp
    labelparser.cpp
    linestyle.cpp
    objectfactory.cpp
    painter.cpp
    palette.cpp
    plottickcalculator.cpp
    psd.cpp
    psdcalculator.cpp
    psdfactory.cpp
    relation.cpp
    relationfactory.cpp
    relationscriptinterface.cpp
)

target_link_libraries(Kst6Math PUBLIC
    Qt6::Widgets
    Qt6::Xml
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Svg
    KF6::I18n
    Kst6Core
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kstmath_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Kst6Math
    COMPONENT Devel
)

install(TARGETS Kst6Math EXPORT KstTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
