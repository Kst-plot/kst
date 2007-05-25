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

#include "kstplotview.h"
#include "kstmainwindow.h"
#include "kstapplication.h"

#include <QDebug>
#include <QUndoStack>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

KstPlotView::KstPlotView()
    : QGraphicsView(kstApp->mainWindow()),
      _currentPlotItem(0), _mouseMode(Default) {

  _undoStack = new QUndoStack(this);
  setScene(new QGraphicsScene(this));
  scene()->installEventFilter(this);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
}


KstPlotView::~KstPlotView() {
}


QUndoStack *KstPlotView::undoStack() const {
  return _undoStack;
}


KstPlotItem *KstPlotView::currentPlotItem() const {
  return _currentPlotItem;
}


KstPlotView::MouseMode KstPlotView::mouseMode() const {
  return _mouseMode;
}


void KstPlotView::setMouseMode(MouseMode mode) {

  if (_mouseMode == Create) {
    _creationPolygonPress.clear();
    _creationPolygonRelease.clear();
    _creationPolygonMove.clear();
  }

  _mouseMode = mode;
}


QPolygonF KstPlotView::creationPolygon(CreationEvents events) const {
#if 0
  QPolygonF resultSet;
  if (events & KstPlotView::MousePress)
    resultSet = resultSet.united(_creationPolygonPress);
  if (events & KstPlotView::MouseRelease)
    resultSet = resultSet.united(_creationPolygonRelease);
  if (events & KstPlotView::MouseMove)
    resultSet = resultSet.united(_creationPolygonMove);
  return resultSet;
#endif
  if (events == KstPlotView::MousePress)
     return _creationPolygonPress;
  if (events == KstPlotView::MouseRelease)
     return _creationPolygonRelease;
  if (events == KstPlotView::MouseMove)
     return _creationPolygonMove;
  return QPolygonF();
}


bool KstPlotView::eventFilter(QObject *obj, QEvent *event) {
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


void KstPlotView::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);
  setSceneRect(QRectF(0, 0, event->size().width(), event->size().height()));

  //See what I'm doing
  QLinearGradient l(0,0,0,event->size().height());
  l.setColorAt(0, Qt::white);
  l.setColorAt(1, Qt::lightGray);
  setBackgroundBrush(l);

  emit resized();
}

#include "kstplotview.moc"

// vim: ts=2 sw=2 et
