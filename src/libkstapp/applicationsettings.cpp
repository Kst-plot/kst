/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "applicationsettings.h"

#include <QCoreApplication>
#include <QGLPixelBuffer>
#include <QSettings>

#include <QDebug>
#ifndef Q_WS_WIN32
#include <QX11Info>
#endif

namespace Kst {

static ApplicationSettings *_self = 0;
void ApplicationSettings::cleanup() {
  delete _self;
  _self = 0;
}


ApplicationSettings *ApplicationSettings::self() {
  if (!_self) {
    _self = new ApplicationSettings;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


ApplicationSettings::ApplicationSettings() {

  _settings = new QSettings("kstapplicationrc", QSettings::NativeFormat);

  //FIXME Not sure if this is the best test for hardware acceleration
  // but right now opening with QGV with QGLWidget as viewport takes
  // several seconds delay when opening application on my system.
  _useOpenGL = _settings->value("general/opengl", QVariant(QGLPixelBuffer::hasOpenGLPbuffers())).toBool();

  _refViewWidth = _settings->value("general/referenceviewwidth", QVariant(16)).toDouble();
  _refViewHeight = _settings->value("general/referenceviewheight", QVariant(12)).toDouble();
  _refFontSize = _settings->value("general/referencefontsize", QVariant(12)).toInt();
  _minFontSize = _settings->value("general/minimumfontsize", QVariant(5)).toInt();
  _defaultFontFamily = _settings->value("general/defaultfontfamily", "Albany AMT").toString();

  _showGrid = _settings->value("grid/showgrid", QVariant(true)).toBool();
  _snapToGrid = _settings->value("grid/snaptogrid", QVariant(false)).toBool();
  _gridHorSpacing = _settings->value("grid/horizontalspacing", 20.0).toDouble();
  _gridVerSpacing = _settings->value("grid/verticalspacing", 20.0).toDouble();
}


ApplicationSettings::~ApplicationSettings() {
  delete _settings;
}


bool ApplicationSettings::useOpenGL() const {
  return _useOpenGL;
}


void ApplicationSettings::setUseOpenGL(bool useOpenGL) {
  _useOpenGL = useOpenGL;
  _settings->setValue("general/opengl", useOpenGL);
  emit modified();
}


int ApplicationSettings::referenceViewWidth() const {
  double dpiScale = 72.0;
#ifndef Q_WS_WIN32
    dpiScale = QX11Info::appDpiX();
#endif
  return (_refViewWidth * dpiScale * 0.3937008);
}


double ApplicationSettings::referenceViewWidthCM() const {
  return _refViewWidth;
}


void ApplicationSettings::setReferenceViewWidthCM(const double width) {
  _refViewWidth = width;
  _settings->setValue("general/referenceviewwidth", width);
  emit modified();
}


int ApplicationSettings::referenceViewHeight() const {
  double dpiScale = 72.0;
#ifndef Q_WS_WIN32
    dpiScale = QX11Info::appDpiY();
#endif
  return (_refViewHeight * dpiScale * 0.3937008);
}


double ApplicationSettings::referenceViewHeightCM() const {
  return _refViewHeight;
}


void ApplicationSettings::setReferenceViewHeightCM(const double height) {
  _refViewHeight = height;
  _settings->setValue("general/referenceviewheight", height);
  emit modified();
}


int ApplicationSettings::referenceFontSize() const {
  return _refFontSize;
}


void ApplicationSettings::setReferenceFontSize(int points) {
  _refFontSize = points;
  _settings->setValue("general/referencefontsize", points);
  emit modified();
}


int ApplicationSettings::minimumFontSize() const {
  return _minFontSize;
}


void ApplicationSettings::setMinimumFontSize(int points) {
  _minFontSize = points;
  _settings->setValue("general/minimumfontsize", points);
  emit modified();
}


QString ApplicationSettings::defaultFontFamily() const {
  return _defaultFontFamily;
}


void ApplicationSettings::setDefaultFontFamily(const QString &fontFamily) {
  _defaultFontFamily = fontFamily;
  _settings->setValue("general/defaultfontfamily", fontFamily);
  emit modified();
}


bool ApplicationSettings::showGrid() const {
  return _showGrid;
}


void ApplicationSettings::setShowGrid(bool showGrid) {
  _showGrid = showGrid;
  _settings->setValue("grid/showgrid", showGrid);
  emit modified();
}


bool ApplicationSettings::snapToGrid() const {
  return _snapToGrid;
}


void ApplicationSettings::setSnapToGrid(bool snapToGrid) {
  _snapToGrid = snapToGrid;
  _settings->setValue("grid/snaptogrid", snapToGrid);
  emit modified();
}


qreal ApplicationSettings::gridHorizontalSpacing() const {
  return _gridHorSpacing;
}


void ApplicationSettings::setGridHorizontalSpacing(qreal spacing) {
  _gridHorSpacing = spacing;
  _settings->setValue("grid/horizontalspacing", spacing);
  emit modified();
}


qreal ApplicationSettings::gridVerticalSpacing() const {
  return _gridVerSpacing;
}


void ApplicationSettings::setGridVerticalSpacing(qreal spacing) {
  _gridVerSpacing = spacing;
  _settings->setValue("grid/verticalspacing", spacing);
  emit modified();
}

}


// vim: ts=2 sw=2 et
