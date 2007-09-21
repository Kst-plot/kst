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
  //FIXME Not sure if this is the best test for hardware acceleration
  // but right now opening with QGV with QGLWidget as viewport takes
  // several seconds delay when opening application on my system.
  _useOpenGL = QGLPixelBuffer::hasOpenGLPbuffers();

  _refViewWidth = 10.0; //cm
  _refViewHeight = 7.0; //cm
  _refFontSize = 16; //points
  _minFontSize = 10; //points

  _showGrid = true;
  _snapToGrid = false;
  _gridHorSpacing = 20.0;
  _gridVerSpacing = 20.0;
}


ApplicationSettings::~ApplicationSettings() {
}


bool ApplicationSettings::useOpenGL() const {
  return _useOpenGL;
}


void ApplicationSettings::setUseOpenGL(bool useOpenGL) {
  _useOpenGL = useOpenGL;
  emit modified();
}


qreal ApplicationSettings::referenceViewWidth() const {
  return _refViewWidth;
}


void ApplicationSettings::setReferenceViewWidth(qreal width) {
  _refViewWidth = width;
  emit modified();
}


qreal ApplicationSettings::referenceViewHeight() const {
  return _refViewHeight;
}


void ApplicationSettings::setReferenceViewHeight(qreal height) {
  _refViewHeight = height;
  emit modified();
}


int ApplicationSettings::referenceFontSize() const {
  return _refFontSize;
}


void ApplicationSettings::setReferenceFontSize(int points) {
  _refFontSize = points;
  emit modified();
}


int ApplicationSettings::minimumFontSize() const {
  return _minFontSize;
}


void ApplicationSettings::setMinimumFontSize(int points) {
  _minFontSize = points;
  emit modified();
}

bool ApplicationSettings::showGrid() const {
  return _showGrid;
}


void ApplicationSettings::setShowGrid(bool showGrid) {
  _showGrid = showGrid;
  emit modified();
}


bool ApplicationSettings::snapToGrid() const {
  return _snapToGrid;
}


void ApplicationSettings::setSnapToGrid(bool snapToGrid) {
  _snapToGrid = snapToGrid;
  emit modified();
}


qreal ApplicationSettings::gridHorizontalSpacing() const {
  return _gridHorSpacing;
}


void ApplicationSettings::setGridHorizontalSpacing(qreal spacing) {
  _gridHorSpacing = spacing;
  emit modified();
}


qreal ApplicationSettings::gridVerticalSpacing() const {
  return _gridVerSpacing;
}


void ApplicationSettings::setGridVerticalSpacing(qreal spacing) {
  _gridVerSpacing = spacing;
  emit modified();
}

}


// vim: ts=2 sw=2 et
