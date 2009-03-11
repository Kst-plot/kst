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

#include "primitive.h"
#include "datasource.h"

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
  _maxUpdate = MAX_UPDATES;
  _paused = false;
  QTimer::singleShot(_maxUpdate, this, SLOT(allowUpdates()));
}


UpdateManager::~UpdateManager() {
}


void UpdateManager::requestUpdate(ObjectPtr object) {
  if (!_updateRequests.contains(object)) {
    _updateRequests.append(object);
  }
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "UM - Update requested for" << object->Name() << "Update Count" << _updateRequests;
#endif
}


void UpdateManager::requestUpdate(ObjectPtr updateObject, ObjectPtr object) {
  if (!_dependentUpdateRequests.contains(updateObject)) {
    QList<ObjectPtr> newList;
    newList.append(object);
    _dependentUpdateRequests.insert(updateObject, newList);
  } else {
    if (!_dependentUpdateRequests[updateObject].contains(object)) {
      _dependentUpdateRequests[updateObject].append(object);
    }
  }
#if DEBUG_UPDATE_CYCLE > 0
  qDebug() << "\t\t\tUM - Curve" << object->shortName() << "requested update requested for" << updateObject->Name();
  qDebug() << "\t\t\t     Current dependent update list" <<  _dependentUpdateRequests;
#endif
}


void UpdateManager::requestUpdate(ObjectPtr updateObject, PlotItemInterface* displayObject) {
  if (!_displayUpdateRequests.contains(updateObject)) {
    QList<PlotItemInterface*> newList;
    newList.append(displayObject);
    _displayUpdateRequests.insert(updateObject, newList);
  } else {
    if (!_displayUpdateRequests[updateObject].contains(displayObject)) {
      _displayUpdateRequests[updateObject].append(displayObject);
    }
  }
#if DEBUG_UPDATE_CYCLE > 0
  qDebug() << "\t\t\tUM - Plot update requested for" << updateObject->Name();
  qDebug() << "\t\t\t     Current display update list" <<  _displayUpdateRequests;
#endif
}


void UpdateManager::allowUpdates() {
#if DEBUG_UPDATE_CYCLE > 0
  qDebug() << "UM - Allow updates triggered";
#endif
  if (!_updateRequests.empty()) {
    if (!_activeUpdates.empty()) {
#if DEBUG_UPDATE_CYCLE > 0
      qDebug() << "UM - Update in progress, delaying start of update";
#endif
      _delayedUpdate = true;
    } else {
      _delayedUpdate = false;
      foreach(ObjectPtr object, _updateRequests) {
#if DEBUG_UPDATE_CYCLE > 0
        qDebug() << "UM - Beginning update for" << object->Name();
#endif
        object->beginUpdate(object);
        _updateRequests.removeAll(object);
      }
    }
  }
#if DEBUG_UPDATE_CYCLE > 0
  qDebug() << "UM - Delaying for " << _maxUpdate << "ms before allowing next update";
#endif
  QTimer::singleShot(_maxUpdate, this, SLOT(allowUpdates()));
}


void UpdateManager::objectDeleted(ObjectPtr object) {
  _updateRequests.removeAll(object);
  _activeUpdates.remove(object);
}


void UpdateManager::updateStarted(ObjectPtr updateObject, ObjectPtr reportingObject) {
  _activeUpdates[updateObject]++;
#if DEBUG_UPDATE_CYCLE > 0
    if (PrimitivePtr primitive = kst_cast<Primitive>(reportingObject)) {
      qDebug() << "\t\tUM - Update beginning for" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    } else if (DataSourcePtr ds = kst_cast<DataSource>(reportingObject)) {
      qDebug() << "\tUM - Update beginning for" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    } else {
      qDebug() << "\t\tUM - Update beginning for" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    }
#else
  Q_UNUSED(reportingObject)
#endif
}


void UpdateManager::updateFinished(ObjectPtr updateObject, ObjectPtr reportingObject) {
  _activeUpdates[updateObject]--;
#if DEBUG_UPDATE_CYCLE > 0
    if (PrimitivePtr primitive = kst_cast<Primitive>(reportingObject)) {
      qDebug() << "\t\tUM - Update finish notification from" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    } else if (DataSourcePtr ds = kst_cast<DataSource>(reportingObject)) {
      qDebug() << "\tUM - Update finish notification from" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    } else {
      qDebug() << "\t\t\tUM - Update finish notification from" << reportingObject->shortName() << "for update" << updateObject->shortName() << "update count" << _activeUpdates[updateObject];
    }
#else
  Q_UNUSED(reportingObject)
#endif
  if (_activeUpdates[updateObject] == 0) {
    _activeUpdates.remove(updateObject);

  // Add Logic for no curves to update plots.
    if (!_dependentUpdateRequests[updateObject].empty()) {
      if (_dispatchingRequests.contains(updateObject)) {
        return;
      }
#if DEBUG_UPDATE_CYCLE > 0
        qDebug() << "\tUM - All primitive updates complete updating relations for update of" <<  updateObject->shortName();
        qDebug() << "\t     Current dependentUpdate requests" << _dependentUpdateRequests;
#endif
      _dispatchingRequests.append(updateObject);
      foreach (ObjectPtr object, _dependentUpdateRequests[updateObject]) {
        _dependentUpdateRequests[updateObject].removeAll(object);
        bool continueWaiting = false;
        foreach (QList<ObjectPtr> list, _dependentUpdateRequests) {
          if (list.contains(object)) {
            continueWaiting = true;
#if DEBUG_UPDATE_CYCLE > 0
            qDebug() << "\t\tUM - Delaying relation update for" << object->shortName() << "object is part of another update.";
#endif
            break;
          }
        }
        if (!continueWaiting) {
          object->beginUpdate(updateObject); 
        }
      }
      _dependentUpdateRequests.remove(updateObject);
      _dispatchingRequests.removeAll(updateObject);
    } else {
      // Display level update required.
      if (_activeUpdates.empty()) {
#if DEBUG_UPDATE_CYCLE > 0
        qDebug() << "\tUM - All relation updates complete, updating plots for update of" <<  updateObject->shortName();
        qDebug() << "\t     Current display update list" <<  _displayUpdateRequests;
#endif
        foreach (QList<PlotItemInterface*> objectList, _displayUpdateRequests) {
          foreach (PlotItemInterface* object, objectList) {
            object->updateObject();
          }
        }
        _displayUpdateRequests.clear();
#if DEBUG_UPDATE_CYCLE > 0
        qDebug() << "\tUM - All Plot updates completed for update of " << updateObject->Name();
        qDebug() << "UM - Update Complete for " << updateObject->Name();
      } else {
        qDebug() << "UM - updates not complete: not updating plots: count:" << _updateRequests.count();
#endif
      }
    }
  }
}

}


// vim: ts=2 sw=2 et
