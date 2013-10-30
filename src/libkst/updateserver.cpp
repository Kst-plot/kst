/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2013 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "updateserver.h"
#include <qapplication.h>

namespace Kst {

UpdateServer *UpdateServer::_self = UpdateServer::self();


UpdateServer* UpdateServer::self() {
  if (!_self) {
    _self = new UpdateServer;
    qAddPostRoutine(UpdateServer::cleanup);
  }
  return _self;
}


void UpdateServer::cleanup() {
    delete _self;
    _self = 0;
}

UpdateServer::UpdateServer(): _signalSuspendDepthCounter(0) {

}

UpdateServer::~UpdateServer() {
}


void UpdateServer::endSuspendSignals() {
  if (_signalSuspendDepthCounter>0) {
    _signalSuspendDepthCounter--;
  }
  if (_signalRequested) {
    requestUpdateSignal();
  }
}

void UpdateServer::requestUpdateSignal() {
  if (_signalSuspendDepthCounter==0) {
    emit objectListsChanged();
    _signalRequested = false;
  } else {
    _signalRequested = true;
  }
}

}
