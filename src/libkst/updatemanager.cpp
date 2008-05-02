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

#include "updatemanager.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#define MAX_UPDATES 2000

namespace Kst {

static UpdateManager *_self = 0;
void UpdateManager::cleanup() {
  delete _self;
  _self = 0;
}


UpdateManager *UpdateManager::self() {
  if (!_self) {
    _self = new UpdateManager;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


UpdateManager::UpdateManager() {
  QTimer::singleShot(MAX_UPDATES, this, SLOT(allowUpdates()));
}


UpdateManager::~UpdateManager() {
}


void UpdateManager::requestUpdate(ObjectPtr object) {
  if (!_updateRequests.contains(object)) {
    _updateRequests.append(object);
  }
  qDebug() << "UPDATE MANAGER - update requested for" << object->tag().displayString() << _updateRequests;
}


void UpdateManager::allowUpdates() {
#if DEBUG_UPDATE_CYCLE
  qDebug() << "UPDATE MANAGER - allow updates triggered";
#endif
  if (!_updateRequests.empty()) {
    if (!_activeUpdates.empty()) {
#if DEBUG_UPDATE_CYCLE
      qDebug() << "UPDATE MANAGER - Update in progress, delaying start of update";
#endif
      _delayedUpdate = true;
    } else {
      _delayedUpdate = false;
      foreach(ObjectPtr object, _updateRequests) {
#if DEBUG_UPDATE_CYCLE
        qDebug() << "UPDATE MANAGER - allowing update to run for" << object->tag().displayString();
#endif
//         _activeUpdates.insert(object, 1);
        object->beginUpdate();
        _updateRequests.removeAll(object);
      }
    }
  }
  QTimer::singleShot(MAX_UPDATES, this, SLOT(allowUpdates()));
}


void UpdateManager::objectDeleted(ObjectPtr object) {
  _updateRequests.removeAll(object);
  _activeUpdates.remove(object);
}


void UpdateManager::updateStarted(ObjectPtr updateObject, ObjectPtr reportingObject) {
//   _activeUpdates[updateObject]++;
}


void UpdateManager::updateFinished(ObjectPtr updateObject, ObjectPtr reportingObject) {
//   _activeUpdates[updateObject]--;
//   if (_activeUpdates[updateObject] == 0) {
//     qDebug() << "update complete for" << updateObject->shortName();
//     _activeUpdates.remove(updateObject);
//   }
}

}


// vim: ts=2 sw=2 et
