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

#include "viewitem.h"
#include "kstapplication.h"
#include "tabwidget.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent) {
  setAcceptsHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  connect(parent, SIGNAL(mouseModeChanged(View::MouseMode)),
          this, SLOT(viewMouseModeChanged(View::MouseMode)));
}


ViewItem::~ViewItem() {
}


View *ViewItem::parentView() const {
  return qobject_cast<View*>(parent());
}


ViewItem::MouseMode ViewItem::mouseMode() const {
  return _mouseMode;
}


void ViewItem::setMouseMode(MouseMode mode) {
  _mouseMode = mode;
}


void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QRectF realBound = mapToScene(boundingRect()).boundingRect();

  painter->save();
  painter->setPen(Qt::DotLine);
  painter->setTransform(parentView()->viewportTransform());
  if (isSelected())
    painter->drawRect(realBound);

#ifdef DEBUG_GEOMETRY
  QColor semiRed(QColor(255, 0, 0, 50));
  painter->fillRect(realBound, semiRed);
#endif

  painter->restore();

  painter->setPen(pen());
  painter->setBrush(brush());
  painter->drawRect(rect());

//   QGraphicsRectItem::paint(painter, option, widget);
}


void ViewItem::remove() {
  RemoveCommand *remove = new RemoveCommand(this);
  remove->redo();
}


void ViewItem::raise() {
  RaiseCommand *up = new RaiseCommand(this);
  up->redo();
}


void ViewItem::lower() {
  LowerCommand *down = new LowerCommand(this);
  down->redo();
}


void ViewItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setRect(poly.first().x(), poly.first().y(),
            poly.last().x() - poly.first().x(), poly.last().y() - poly.first().y());
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    setRect(rect().x(), rect().y(),
            poly.last().x() - rect().x(), poly.last().y() - rect().y());
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    setRect(rect().x(), rect().y(),
            poly.last().x() - rect().x(), poly.last().y() - rect().y());

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    emit creationComplete();
    return;
  }
}


void ViewItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  QAction *removeAction = menu.addAction(tr("Remove"));
  connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  QAction *raiseAction = menu.addAction(tr("Raise"));
  connect(raiseAction, SIGNAL(triggered()), this, SLOT(raise()));

  QAction *lowerAction = menu.addAction(tr("Lower"));
  connect(lowerAction, SIGNAL(triggered()), this, SLOT(lower()));

  menu.exec(event->screenPos());
}


void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->mouseMode() == View::Default) {
    if (mouseMode() == ViewItem::Default ||
        mouseMode() == ViewItem::Move) {
      parentView()->setMouseMode(View::Move);
      parentView()->undoStack()->beginMacro(tr("Move"));
    } else if (mouseMode() == ViewItem::Resize) {
      parentView()->setMouseMode(View::Resize);
      parentView()->undoStack()->beginMacro(tr("Resize"));
    } else if (mouseMode() == ViewItem::Rotate) {
      parentView()->setMouseMode(View::Rotate);
      parentView()->undoStack()->beginMacro(tr("Rotate"));
    }
  }

  switch(cursor().shape()) {
  case Qt::SizeFDiagCursor:
    {
      if (event->pos().x() < rect().center().x()) {
        if (_mouseMode == Resize)
          setTopLeft(event->pos());
        else if (_mouseMode == Rotate)
          rotateTowards(rect().topLeft(), event->pos());
      } else {
        if (_mouseMode == Resize)
          setBottomRight(event->pos());
        else if (_mouseMode == Rotate)
          rotateTowards(rect().bottomRight(), event->pos());
      }
      return;
    }
  case Qt::SizeBDiagCursor:
    {
      if (event->pos().x() < rect().center().x()) {
        if (_mouseMode == Resize)
          setBottomLeft(event->pos());
        else if (_mouseMode == Rotate)
          rotateTowards(rect().bottomLeft(), event->pos());
      } else {
        if (_mouseMode == Resize)
          setTopRight(event->pos());
        else if (_mouseMode == Rotate)
          rotateTowards(rect().topRight(), event->pos());
      }
      return;
    }
  case Qt::SizeVerCursor:
    {
      if (event->pos().y() < rect().center().y()) {
        setTop(event->pos().y());
      } else {
        setBottom(event->pos().y());
      }
      return;
    }
  case Qt::SizeHorCursor:
    {
      if (event->pos().x() < rect().center().x()) {
        setLeft(event->pos().x());
      } else {
        setRight(event->pos().x());
      }
      return;
    }
  case Qt::ArrowCursor:
  default:
    break;
  }

  QGraphicsRectItem::mouseMoveEvent(event);
}


void ViewItem::setTopLeft(const QPointF &point) {
  QRectF transformed = rect();
  transformed.setTopLeft(point);
  transformToRect(transformed);
}


void ViewItem::setTopRight(const QPointF &point) {
  QRectF transformed = rect();
  transformed.setTopRight(point);
  transformToRect(transformed);
}


void ViewItem::setBottomLeft(const QPointF &point) {
  QRectF transformed = rect();
  transformed.setBottomLeft(point);
  transformToRect(transformed);
}


void ViewItem::setBottomRight(const QPointF &point) {
  QRectF transformed = rect();
  transformed.setBottomRight(point);
  transformToRect(transformed);
}


void ViewItem::setTop(qreal x) {
  QRectF transformed = rect();
  transformed.setTop(x);
  transformToRect(transformed);
}


void ViewItem::setBottom(qreal x) {
  QRectF transformed = rect();
  transformed.setBottom(x);
  transformToRect(transformed);
}


void ViewItem::setLeft(qreal x) {
  QRectF transformed = rect();
  transformed.setLeft(x);
  transformToRect(transformed);
}


void ViewItem::setRight(qreal x) {
  QRectF transformed = rect();
  transformed.setRight(x);
  transformToRect(transformed);
}


bool ViewItem::transformToRect(const QRectF &newRect) {

/* setRect(newRect);*/

  QTransform t;
  QPolygonF one(rect());
  one.pop_back(); //get rid of last closed point
  QPolygonF two(newRect);
  two.pop_back(); //get rid of last closed point
  bool success = QTransform::quadToQuad(one, two, t);
  if (success) setTransform(t, true);
  return success;
}


void ViewItem::rotateTowards(const QPointF &corner, const QPointF &point) {
  QPointF origin = rect().center();
  QLineF unit(origin, QPointF(origin.x() + 1, origin.y()));
  QLineF normal(origin, corner);
  QLineF rotated(origin, point);

  /*FIXME better way to check the sign*/
  bool clockWise = unit.angle(rotated) >= unit.angle(normal);

  qreal angle = normal.angle(rotated);
  angle = (clockWise ? angle : -angle);

  QTransform t;
  t.translate(origin.x(), origin.y());
  t.rotate(angle);
  t.translate(-origin.x(), -origin.y());
  setTransform(t, true);
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mousePressEvent(event);
}


void ViewItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->mouseMode() != View::Default) {
    parentView()->setMouseMode(View::Default);
    parentView()->undoStack()->endMacro();
  }

  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);

  QRectF r;
  r.setSize(QSizeF(14,14)); //gives us corners of 7x7

  //Look for corners
  r.moveCenter(rect().bottomRight());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeFDiagCursor);
    return;
  }

  r.moveCenter(rect().topLeft());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeFDiagCursor);
    return;
  }

  r.moveCenter(rect().bottomLeft());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeBDiagCursor);
    return;
  }

  r.moveCenter(rect().topRight());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeBDiagCursor);
    return;
  }

  //Now look for horizontal edges
  r.setSize(QSizeF(rect().width(), 7));

  r.moveTopRight(rect().topRight());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeVerCursor);
    return;
  }

  r.moveBottomRight(rect().bottomRight());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeVerCursor);
    return;
  }

  //Now look for vertical edges
  r.setSize(QSizeF(7, rect().height()));

  r.moveTopLeft(rect().topLeft());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeHorCursor);
    return;
  }

  r.moveTopRight(rect().topRight());
  if (r.contains(event->pos())) {
    setMouseMode(ViewItem::Resize);
    setCursor(Qt::SizeHorCursor);
    return;
  }

  setMouseMode(ViewItem::Default);
  setCursor(Qt::ArrowCursor);
}


void ViewItem::keyPressEvent(QKeyEvent *event) {
  QGraphicsRectItem::keyPressEvent(event);
  if (_mouseMode == ViewItem::Resize && event->modifiers() & Qt::ShiftModifier) {
    setMouseMode(ViewItem::Rotate);
  } else if (_mouseMode == ViewItem::Rotate && event->modifiers() & Qt::ShiftModifier) {
    setMouseMode(ViewItem::Resize);
  }
}


void ViewItem::viewMouseModeChanged(View::MouseMode oldMode) {
  if (parentView()->mouseMode() == View::Move) {
    _originalPosition = pos();
  } else if (parentView()->mouseMode() == View::Resize ||
             parentView()->mouseMode() == View::Rotate) {
    _originalTransform = transform();
  } else if (oldMode == View::Move) {
    new MoveCommand(this, _originalPosition, pos());
  } else if (oldMode == View::Resize) {
    new ResizeCommand(this, _originalTransform, transform());
  } else if (oldMode == View::Rotate) {
    new RotateCommand(this, _originalTransform, transform());
  }
}


ViewItemCommand::ViewItemCommand(const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->tabWidget()->currentView()->currentPlotItem()) {
  if (addToStack)
    _item->parentView()->undoStack()->push(this);
}


ViewItemCommand::ViewItemCommand(ViewItem *item, const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
  if (addToStack)
    _item->parentView()->undoStack()->push(this);
}


ViewItemCommand::~ViewItemCommand() {
}


CreateCommand::CreateCommand(const QString &text, QUndoCommand *parent)
    : ViewCommand(text, false, parent) {
}


CreateCommand::CreateCommand(View *view, const QString &text, QUndoCommand *parent)
    : ViewCommand(view, text, false, parent) {
}


CreateCommand::~CreateCommand() {
}


void CreateCommand::undo() {
  if (_item)
    _item->hide();
}


void CreateCommand::redo() {
  if (!_item)
    createItem();

  _item->show();
}


void CreateCommand::createItem() {
  Q_ASSERT(_item);

  _view->setMouseMode(View::Create);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(_view, SIGNAL(mouseModeChanged(View::MouseMode)), _item, SLOT(deleteLater()));
  connect(_view, SIGNAL(creationPolygonChanged(View::CreationEvent)),
          _item, SLOT(creationPolygonChanged(View::CreationEvent)));
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));
  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}


void CreateCommand::creationComplete() {
  _view->undoStack()->push(this);
}


void MoveCommand::undo() {
  _item->setPos(_originalPos);
}


void MoveCommand::redo() {
  _item->setPos(_newPos);
}


void RemoveCommand::undo() {
  _item->show();
}


void RemoveCommand::redo() {
  _item->hide();
}


void RaiseCommand::undo() {
  _item->setZValue(_item->zValue() - 1);
}


void RaiseCommand::redo() {
  _item->setZValue(_item->zValue() + 1);
}


void LowerCommand::undo() {
  _item->setZValue(_item->zValue() +1);
}


void LowerCommand::redo() {
  _item->setZValue(_item->zValue() - 1);
}


void TransformCommand::undo() {
  _item->setTransform(_originalTransform);
}


void TransformCommand::redo() {
  _item->setTransform(_newTransform);
}


}

#include "viewitem.moc"

// vim: ts=2 sw=2 et
