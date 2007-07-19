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

#include "viewmanager.h"

#include "mainwindow.h"
#include "sessionmodel.h"
#include "tabwidget.h"
#include "view.h"

#include <QGraphicsScene>

namespace Kst {

ViewManager::ViewManager(MainWindow *parent)
: QDialog(parent), _win(parent) {
  ui.setupUi(this);
  View *v = _win->tabWidget()->currentView();
  if (v) {
    QGraphicsScene *scene = v->scene();
    // Setup a model for the scene
    //ui.view->setModel(scene);
  }
}


ViewManager::~ViewManager() {
}

}

// vim: ts=2 sw=2 et
