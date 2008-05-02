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

#include <QObject>

namespace Kst {

class UpdateManager : public QObject
{
  Q_OBJECT
  public:
    static UpdateManager *self();

    void requestUpdate(ObjectPtr object);
    void objectDeleted(ObjectPtr object);

    void updateStarted(ObjectPtr updateObject, ObjectPtr reportingObject);
    void updateFinished(ObjectPtr updateObject, ObjectPtr reportingObject);

  private Q_SLOTS:
    void allowUpdates();

  private:
    UpdateManager();
    ~UpdateManager();
    static void cleanup();

  private:
    QList<ObjectPtr> _updateRequests;
    QMap<ObjectPtr, int> _activeUpdates;

    bool _delayedUpdate;
};

}

#endif

// vim: ts=2 sw=2 et
