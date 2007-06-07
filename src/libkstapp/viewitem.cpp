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
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent) {
  setAcceptsHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(mouseModeChanged()));
}


ViewItem::~ViewItem() {
}


View *ViewItem::parentView() const {
  return qobject_cast<View*>(parent());
}


void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

#ifdef DEBUG_GEOMETRY
  QColor semiRed(QColor(255, 0, 0, 50));
  painter->fillRect(boundingRect(), semiRed);
#endif

  QGraphicsRectItem::paint(painter, option, widget);
}


void ViewItem::removeItem() {
  RemoveCommand *remove = new RemoveCommand(this);
  remove->redo();
}


void ViewItem::zOrderUp() {
  ZOrderUpCommand *up = new ZOrderUpCommand(this);
  up->redo();
}


void ViewItem::zOrderDown() {
  ZOrderDownCommand *down = new ZOrderDownCommand(this);
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

  QAction *removeAction = menu.addAction(tr("Remove Object"));
  connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));

  QAction *zUpAction = menu.addAction(tr("Z Order Up"));
  connect(zUpAction, SIGNAL(triggered()), this, SLOT(zOrderUp()));

  QAction *zDownAction = menu.addAction(tr("Z Order Down"));
  connect(zDownAction, SIGNAL(triggered()), this, SLOT(zOrderDown()));

  menu.exec(event->screenPos());
}


void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (scene()->mouseGrabberItem() != this)
    return;

  //We have the mouse grab...
  switch(cursor().shape()) {
  case Qt::SizeFDiagCursor:
    {
      QRectF transformed = rect();
      if (event->pos().x() < rect().center().x()) {
        transformed.setTopLeft(event->pos());
      } else {
        transformed.setBottomRight(event->pos());
      }
/*      setRect(transformed);*/
      transformToRect(transformed);
      return;
    }
  case Qt::SizeBDiagCursor:
    {
      QRectF transformed = rect();
      if (event->pos().x() < rect().center().x()) {
        transformed.setBottomLeft(event->pos());
      } else {
        transformed.setTopRight(event->pos());
      }
/*      setRect(transformed);*/
      transformToRect(transformed);
      return;
    }
  case Qt::SizeVerCursor:
    {
      QRectF transformed = rect();
      if (event->pos().y() < rect().center().y()) {
        transformed.setTop(event->pos().y());
      } else {
        transformed.setBottom(event->pos().y());
      }
/*      setRect(transformed);*/
      transformToRect(transformed);
      return;
    }
  case Qt::SizeHorCursor:
    {
      QRectF transformed = rect();
      if (event->pos().x() < rect().center().x()) {
        transformed.setLeft(event->pos().x());
      } else {
        transformed.setRight(event->pos().x());
      }
/*      setRect(transformed);*/
      transformToRect(transformed);

      return;
    }
  case Qt::ArrowCursor:
  default:
    break;
  }

  QGraphicsRectItem::mouseMoveEvent(event);
}


bool ViewItem::transformToRect(const QRectF &newRect) {
  QTransform t;
  QPolygonF one(rect());
  one.pop_back(); //get rid of last closed point
  QPolygonF two(newRect);
  two.pop_back(); //get rid of last closed point
  bool success = QTransform::quadToQuad(one, two, t);
  if (success) setTransform(t, true);
  return success;
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mousePressEvent(event);
}


void ViewItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);

  QRectF r;
  r.setSize(QSizeF(14,14)); //gives us corners of 7x7

  //Look for corners
  r.moveCenter(rect().bottomRight());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeFDiagCursor);
    return;
  }

  r.moveCenter(rect().topLeft());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeFDiagCursor);
    return;
  }

  r.moveCenter(rect().bottomLeft());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeBDiagCursor);
    return;
  }

  r.moveCenter(rect().topRight());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeBDiagCursor);
    return;
  }

  //Now look for horizontal edges
  r.setSize(QSizeF(rect().width(), 7));

  r.moveTopRight(rect().topRight());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeVerCursor);
    return;
  }

  r.moveBottomRight(rect().bottomRight());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeVerCursor);
    return;
  }

  //Now look for vertical edges
  r.setSize(QSizeF(7, rect().height()));

  r.moveTopLeft(rect().topLeft());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeHorCursor);
    return;
  }

  r.moveTopRight(rect().topRight());
  if (r.contains(event->pos())) {
    setCursor(Qt::SizeHorCursor);
    return;
  }

  setCursor(Qt::ArrowCursor);
}


void ViewItem::mouseModeChanged() {
  if (parentView()->mouseMode() == View::Move)
    _originalPosition = pos();
  else if (_originalPosition != pos())
    new MoveCommand(this, _originalPosition, pos());
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
  connect(_view, SIGNAL(mouseModeChanged()), _item, SLOT(deleteLater()));
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


void ZOrderUpCommand::undo() {
  _item->setZValue(_item->zValue() - 1);
}


void ZOrderUpCommand::redo() {
  _item->setZValue(_item->zValue() + 1);
}


void ZOrderDownCommand::undo() {
  _item->setZValue(_item->zValue() +1);
}


void ZOrderDownCommand::redo() {
  _item->setZValue(_item->zValue() - 1);
}

}

#include "viewitem.moc"

// vim: ts=2 sw=2 et
