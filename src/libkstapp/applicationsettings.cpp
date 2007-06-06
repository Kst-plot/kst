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
}


ApplicationSettings::~ApplicationSettings() {
}


bool ApplicationSettings::useOpenGL() const {
  return _useOpenGL;
}


void ApplicationSettings::setUseOpenGL(bool use) {
  _useOpenGL = use;
}

}


// vim: ts=2 sw=2 et
