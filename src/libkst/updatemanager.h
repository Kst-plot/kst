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

#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include "object.h"
#include "plotiteminterface.h"

#include <QGraphicsRectItem>

// Provides additional output during update cycle.
// Level 0 No Debug
// Level 1 Simple Debugging - UpdateManager Only
// Level 2 Full Debugging
#define DEBUG_UPDATE_CYCLE 0

namespace Kst {

class UpdateManager : public QObject
{
  Q_OBJECT
  public:
    static UpdateManager *self();

    void requestUpdate(ObjectPtr object);
    void requestUpdate(ObjectPtr updateObject, ObjectPtr object);
    void requestUpdate(ObjectPtr updateObject, PlotItemInterface* displayObject);
    void objectDeleted(ObjectPtr object);

    void updateStarted(ObjectPtr updateObject, ObjectPtr reportingObject);
    void updateFinished(ObjectPtr updateObject, ObjectPtr reportingObject);

    void setMinimumUpdatePeriod(const int period) { _maxUpdate = period; }
    int minimumUpdatePeriod() { return _maxUpdate; }

    void setPaused(bool paused) { _paused = paused;}
    bool paused() { return _paused; }
  private Q_SLOTS:
    void allowUpdates();

  private:
    UpdateManager();
    ~UpdateManager();
    static void cleanup();

  private:
    QList<ObjectPtr> _updateRequests;
    QMap<ObjectPtr, int> _activeUpdates;
    QMap<ObjectPtr, QList<ObjectPtr> > _dependentUpdateRequests;
    QMap<ObjectPtr, QList<PlotItemInterface*> > _displayUpdateRequests;
    QList<ObjectPtr> _dispatchingRequests;

    bool _delayedUpdate;
    int _maxUpdate;
    bool _paused;
};

}

#endif

// vim: ts=2 sw=2 et
