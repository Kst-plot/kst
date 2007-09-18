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
#include "viewitem.h"
#include "mainwindow.h"
#include "application.h"
#include "applicationsettings.h"

#include <math.h>

#include <QDebug>
#include <QTimer>
#include <QUndoStack>
#include <QResizeEvent>
#include <QGLWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

namespace Kst {

View::View()
  : QGraphicsView(kstApp->mainWindow()),
    _currentViewItem(0),
    _viewMode(Layout),
    _mouseMode(Default),
    _layoutBoxItem(0),
    _gridSpacing(QSizeF(20,20)),
    _snapToGridHorizontal(false),
    _snapToGridVertical(false) {

  _undoStack = new QUndoStack(this);
  setScene(new QGraphicsScene(this));
  scene()->installEventFilter(this);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  if (ApplicationSettings::self()->useOpenGL()) {
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewport(new QGLWidget);
  }
}


View::~View() {
}


QUndoStack *View::undoStack() const {
  return _undoStack;
}


Kst::ViewItem *View::currentViewItem() const {
  return _currentViewItem;
}


View::ViewMode View::viewMode() const {
  return _viewMode;
}


void View::setViewMode(ViewMode mode) {
  ViewMode oldMode = _viewMode;
  _viewMode = mode;
  emit viewModeChanged(oldMode);
}


View::MouseMode View::mouseMode() const {
  return _mouseMode;
}


void View::setMouseMode(MouseMode mode) {

  //Clear the creation polygons if we're currently
  //in Create mouse mode.
  MouseMode oldMode = _mouseMode;

  if (oldMode == Create) {
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

  emit mouseModeChanged(oldMode);
}


QPolygonF View::creationPolygon(CreationEvents events) const {
  if (events == View::MousePress)
     return _creationPolygonPress;
  if (events == View::MouseRelease)
     return _creationPolygonRelease;
  if (events == View::MouseMove)
     return _creationPolygonMove;
  return QPolygonF();
}


QPointF View::snapPoint(const QPointF &point) {
  qreal x = point.x();
  qreal y = point.y();
  if (_snapToGridHorizontal && gridSpacing().width() > 0)
    x -= fmod(point.x(), gridSpacing().width());

  if (_snapToGridVertical && gridSpacing().height() > 0)
    y -= fmod(point.y(), gridSpacing().height());

  return QPointF(x, y);
}


bool View::eventFilter(QObject *obj, QEvent *event) {
  if (obj != scene() || _mouseMode != Create)
    return QGraphicsView::eventFilter(obj, event);

  switch (event->type()) {
  case QEvent::GraphicsSceneMousePress:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      if (e->button() != Qt::LeftButton) break;
      _creationPolygonPress << snapPoint(e->buttonDownScenePos(Qt::LeftButton));
      emit creationPolygonChanged(MousePress);
      return true; //filter this otherwise something can grab our mouse...
    }
  case QEvent::GraphicsSceneMouseRelease:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      if (e->button() != Qt::LeftButton) break;
      _creationPolygonRelease << snapPoint(e->scenePos());
      emit creationPolygonChanged(MouseRelease);
      break;
    }
  case QEvent::GraphicsSceneMouseMove:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      _creationPolygonMove << snapPoint(e->scenePos());
      emit creationPolygonChanged(MouseMove);
      break;
    }
  default:
    break;
  }

  return QGraphicsView::eventFilter(obj, event);
}


void View::createLayout() {
  if (_layoutBoxItem && _layoutBoxItem->isVisible() && _layoutBoxItem->layout() )
    return;

  CreateLayoutBoxCommand *box = new CreateLayoutBoxCommand(this);
  box->redo();
}


void View::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);

  if (size() != sceneRect().size()) {
    QRectF oldSceneRect = sceneRect();

    setSceneRect(QRectF(0.0, 0.0, width() - 4.0, height() - 4.0));

    QLinearGradient l(0.0, 0.0, 0.0, height() - 4.0);
    l.setColorAt(0.0, Qt::white);
    l.setColorAt(1.0, Qt::lightGray);
    setBackgroundBrush(l);

    foreach (QGraphicsItem *item, items()) {
      if (item->parentItem())
        continue;

      ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
      Q_ASSERT(viewItem);

      ViewItem::updateChildGeometry(viewItem, oldSceneRect, sceneRect());
    }
  }
}


void View::drawBackground(QPainter *painter, const QRectF &rect) {

  QGraphicsView::drawBackground(painter, rect);

  painter->save();
  painter->setPen(Qt::gray);
  painter->setOpacity(0.2);

  const QRectF r = sceneRect();
  qreal spacing = gridSpacing().width();

  //FIXME We should probably only draw those lines that intercept rect

  //vertical lines
  qreal x = r.left() + spacing;
  while (x < r.right() && spacing > 0) {
    QLineF line(QPointF(x, r.top()), QPointF(x, r.bottom()));
    painter->drawLine(line);
    x += spacing;
  }

  spacing = gridSpacing().height();

  //horizontal lines
  qreal y = r.top() + spacing;
  while (y < r.bottom() && spacing > 0) {
    QLineF line(QPointF(r.left(), y), QPointF(r.right(), y));
    painter->drawLine(line);
    y += spacing;
  }

  painter->restore();
}


void View::updateChildGeometry(const QRectF &oldSceneRect) {
  foreach (QGraphicsItem *item, items()) {
    if (item->parentItem())
      continue;

    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    Q_ASSERT(viewItem);

    ViewItem::updateChildGeometry(viewItem, oldSceneRect, sceneRect());
  }
}


}

// vim: ts=2 sw=2 et
