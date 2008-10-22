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

#include <QInputDialog>
#include <QMenu>
#include <QTabBar>
#include <QUndoGroup>
#include <QDebug>

namespace Kst {

TabWidget::TabWidget(QWidget *parent)
: QTabWidget(parent) {
  tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(tabBar(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenu(const QPoint&)));
  _cnt = 0;
}


TabWidget::~TabWidget() {
}


View *TabWidget::createView() {
  View *view = new View;
  connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(viewDestroyed(QObject*)));
  MainWindow *parent = qobject_cast<MainWindow*>(this->parent());
  if (parent) {
    parent->undoGroup()->addStack(view->undoStack());
    parent->undoGroup()->setActiveStack(view->undoStack());
  }

  QString label = view->objectName().isEmpty() ?
                  tr("View %1").arg(++_cnt) :
                  view->objectName();

  addTab(view, label);
  setCurrentWidget(view);
  return view;
}


View *TabWidget::currentView() const {
  return qobject_cast<View*>(currentWidget());
}


QList<View*> TabWidget::views() const {
  QList<View*> v;
  for (int i = 0; i < count(); ++i) {
    v.append(qobject_cast<View*>(widget(i)));
  }
  return v;
}


void TabWidget::viewDestroyed(QObject *object) {
  View *view = qobject_cast<View*>(object);
  removeTab(indexOf(view));
}


void TabWidget::closeCurrentView() {
  if (count() == 1) {
    _cnt = 0;
    createView();
    setCurrentIndex(0);
  }
  delete currentView();
}


void TabWidget::renameCurrentView() {
  QTabBar *tb = tabBar();
  int idx = tb->currentIndex();
  bool ok = false;
  QString rc = QInputDialog::getText(this, tr("Rename Tab"), tr("Enter a new tab name:"), QLineEdit::Normal, tb->tabText(idx), &ok);
  if (ok) {
    tb->setTabText(idx, rc);
  }
}


void TabWidget::contextMenu(const QPoint& pos) {
  QTabBar *tb = tabBar();
  int idx = tb->currentIndex();
  for (int i = 0; i < tb->count(); ++i) {
    if (tb->tabRect(i).contains(pos)) {
       idx = i;
       tb->setCurrentIndex(i);
       break;
    }
  }
  const QString txt = tb->tabText(idx);
  QMenu m(txt, tb);
  m.addAction(tr("&Add tab"), this, SLOT(createView()));
  m.addAction(tr("&Rename tab"), this, SLOT(renameCurrentView()));
  m.addAction(tr("&Close tab"), this, SLOT(closeCurrentView()));
  m.exec(tb->mapToGlobal(pos));
}

}

// vim: ts=2 sw=2 et
