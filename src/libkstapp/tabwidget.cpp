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
#include "viewitem.h"
#include "curveplacement.h"


#include <QInputDialog>
#include <QMenu>
#include <QTabBar>
#include <QUndoGroup>
#include <QDebug>

namespace Kst {

class TabBar : public QTabBar
{
public:
  TabBar(TabWidget* tabw);

  void dragEnterEvent(QDragEnterEvent* event);
  void dragMoveEvent(QDragMoveEvent* event);
  void dropEvent(QDropEvent* event);

  TabWidget* tabWidget;
};

TabBar::TabBar(TabWidget* tabw) : QTabBar(), tabWidget(tabw)
{
  setAcceptDrops(true);
}

void TabBar::dragEnterEvent(QDragEnterEvent* event)
{
  if (MimeDataViewItem::downcast(event->mimeData())) {
      event->acceptProposedAction();
  }
}

void TabBar::dragMoveEvent(QDragMoveEvent* event)
{  
  if (MimeDataViewItem::downcast(event->mimeData())) {
    setCurrentIndex(tabAt(event->pos()));
    event->acceptProposedAction();
   }
}

void TabBar::dropEvent(QDropEvent* event)
{
  const MimeDataViewItem* m = MimeDataViewItem::downcast(event->mimeData());
  if (m && m->item) {       
    View* view = tabWidget->currentView();
    view->appendToLayout(CurvePlacement::Auto, m->item);
    event->acceptProposedAction();
  }
}



TabWidget::TabWidget(QWidget *parent)
: QTabWidget(parent) {
  setTabBar(new TabBar(this));
  tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(tabBar(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenu(const QPoint&)));
  _cnt = 0;
#if QT_VERSION >= 0x040500
  tabBar()->setMovable(true);
#endif
}


TabWidget::~TabWidget() {
}


View *TabWidget::createView() {
  View *view = new View;
  addView(view);
  return view;
}


void TabWidget::addView(View* view) {
  MainWindow *parent = qobject_cast<MainWindow*>(this->parent());
  if (parent) {
    parent->undoGroup()->addStack(view->undoStack());
    parent->undoGroup()->setActiveStack(view->undoStack());
  }

  QString label = view->objectName().isEmpty() ?
                  tr("View &%1").arg(++_cnt) :
                  view->objectName();

  addTab(view, label);
  setCurrentWidget(view);
  tabBar()->setAcceptDrops(true);
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


void TabWidget::deleteView(View* view) {
  MainWindow *parent = qobject_cast<MainWindow*>(this->parent());
  if (parent) {
    parent->undoGroup()->removeStack(view->undoStack());
  }
  removeTab(indexOf(view));
  delete view;
}


void TabWidget::clear() {
  QList<View*> tabs = views();
  foreach(View* view, tabs) {
    deleteView(view);
  }
  _cnt = 0;
}


void TabWidget::closeCurrentView() {
  deleteView(currentView());
  if (count() == 0)
    createView();
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

void TabWidget::setCurrentViewName(QString name) {
  int idx = tabBar()->currentIndex();
  tabBar()->setTabText(idx, name);
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
  m.addSeparator();
  m.addAction(tr("&Edit View"), currentView(), SLOT(edit()));

  QMenu layoutMenu;
  layoutMenu.setTitle(tr("Cleanup Layout"));
  layoutMenu.addAction(tr("Automatic"), currentView(), SLOT(createLayout()));
  layoutMenu.addAction(tr("Custom"), currentView(), SLOT(createCustomLayout()));
  m.addMenu(&layoutMenu);

  m.exec(tb->mapToGlobal(pos));
}

}

// vim: ts=2 sw=2 et
