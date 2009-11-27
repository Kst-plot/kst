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

#include <QGraphicsRectItem>

namespace Kst {
class ObjectStore;
class TabWidget;

class UpdateManager : public QObject
{
  Q_OBJECT
  public:
    static UpdateManager *self();

    void setMinimumUpdatePeriod(const int period) { _maxUpdate = period; }
    int minimumUpdatePeriod() { return _maxUpdate; }

    void setPaused(bool paused) { _paused = paused;}
    bool paused() { return _paused; }

    void setStore(ObjectStore *store) {_store = store;}
    void setTabWidget(TabWidget *tabWidget) {_tabWidget = tabWidget;}

  public Q_SLOTS:
    void doUpdates(bool forceImmediate = false);

  Q_SIGNALS:
    void objectsUpdated(qint64 serial);

  private:
    UpdateManager();
    ~UpdateManager();
    static void cleanup();

  private:
    bool _delayedUpdate;
    int _maxUpdate;
    bool _paused;
    qint64 _serial;
    ObjectStore *_store;
    TabWidget *_tabWidget;
};

}

#endif

// vim: ts=2 sw=2 et
