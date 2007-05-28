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

#include "tabwidget.h"
#include "mainwindow.h"
#include "view.h"

#include <QUndoGroup>

namespace Kst {

TabWidget::TabWidget(QWidget *parent)
: QTabWidget(parent) {
}


TabWidget::~TabWidget() {
}


View *TabWidget::createView() {
  View *view = new View;
  connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(viewDestroyed(QObject*)));
  MainWindow *parent = qobject_cast<Kst::MainWindow*>(this->parent());
  if (parent) {
    parent->undoGroup()->addStack(view->undoStack());
    parent->undoGroup()->setActiveStack(view->undoStack());
  }

  static int cnt = 1;
  QString label = view->objectName().isEmpty() ?
                  tr("Plot %1").arg(cnt++) :
                  view->objectName();

  addTab(view, label);
  setCurrentWidget(view);
  return view;
}


View *TabWidget::currentView() const {
  return qobject_cast<Kst::View*>(currentWidget());
}


void TabWidget::viewDestroyed(QObject *object) {
  View *view = qobject_cast<View*>(object);
  removeTab(indexOf(view));
}


void TabWidget::closeCurrentView() {
  delete currentView();
  if (count() == 0) {
    createView();
  }
}


}


#include "tabwidget.moc"

// vim: ts=2 sw=2 et
