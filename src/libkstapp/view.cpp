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

#include "view.h"
#include "mainwindow.h"
#include "kstapplication.h"

#include <QDebug>
#include <QTimer>
#include <QUndoStack>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

namespace Kst {
View::View()
    : QGraphicsView(kstApp->mainWindow()),
      _currentPlotItem(0), _mouseMode(Default) {

  _undoStack = new QUndoStack(this);
  setScene(new QGraphicsScene(this));
  scene()->installEventFilter(this);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
}


View::~View() {
}


QUndoStack *View::undoStack() const {
  return _undoStack;
}


Kst::ViewItem *View::currentPlotItem() const {
  return _currentPlotItem;
}


View::MouseMode View::mouseMode() const {
  return _mouseMode;
}


void View::setMouseMode(MouseMode mode) {

  if (_mouseMode == Create) {
    _creationPolygonPress.clear();
    _creationPolygonRelease.clear();
    _creationPolygonMove.clear();
  }

  _mouseMode = mode;

  if (_mouseMode != Create) {
    setCursor(Qt::ArrowCursor);
    setDragMode(QGraphicsView::RubberBandDrag);
  } else {
    setDragMode(QGraphicsView::NoDrag);
  }

  emit mouseModeChanged();
}


QPolygonF View::creationPolygon(CreationEvents events) const {
#if 0
  QPolygonF resultSet;
  if (events & View::MousePress)
    resultSet = resultSet.united(_creationPolygonPress);
  if (events & View::MouseRelease)
    resultSet = resultSet.united(_creationPolygonRelease);
  if (events & View::MouseMove)
    resultSet = resultSet.united(_creationPolygonMove);
  return resultSet;
#endif
  if (events == View::MousePress)
     return _creationPolygonPress;
  if (events == View::MouseRelease)
     return _creationPolygonRelease;
  if (events == View::MouseMove)
     return _creationPolygonMove;
  return QPolygonF();
}


bool View::eventFilter(QObject *obj, QEvent *event) {
  if (obj != scene() || _mouseMode != Create)
    return QGraphicsView::eventFilter(obj, event);

  switch (event->type()) {
  case QEvent::GraphicsSceneMousePress:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      _creationPolygonPress << e->buttonDownScenePos(Qt::LeftButton);
      emit creationPolygonChanged(MousePress);
      return false;
    }
  case QEvent::GraphicsSceneMouseRelease:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      _creationPolygonRelease << e->scenePos();
      emit creationPolygonChanged(MouseRelease);
      return false;
    }
  case QEvent::GraphicsSceneMouseMove:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      _creationPolygonMove << e->scenePos();
      emit creationPolygonChanged(MouseMove);
      return false;
    }
  default:
    return QGraphicsView::eventFilter(obj, event);
  }
}


void View::setVisible(bool visible) {
  QGraphicsView::setVisible(visible);
  QTimer::singleShot(0, this, SLOT(initializeSceneRect()));
}


void View::initializeSceneRect() {

  //Maybe this should be the size of the desktop?
  setSceneRect(QRectF(0, 0, width() - 1.0, height() - 1.0));

  //See what I'm doing
  QLinearGradient l(0,0,0,height());
  l.setColorAt(0, Qt::white);
  l.setColorAt(1, Qt::lightGray);
  setBackgroundBrush(l);
}


void View::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);

  if (size() != sceneRect().size() && sceneRect().isValid()) {
    fitInView(sceneRect());
  }
}
}

#include "view.moc"

// vim: ts=2 sw=2 et
