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
#include "viewitemdialog.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent), _mouseMode(Default), _layout(0), _activeGrip(NoGrip) {
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
  update();
}


ViewGridLayout *ViewItem::layout() const {
  return _layout;
}


void ViewItem::setLayout(ViewGridLayout *layout) {
  _layout = layout;
}


QRectF ViewItem::viewRect() const {
  return rect();
}


void ViewItem::setViewRect(const QRectF &viewRect) {
  setRect(viewRect);
  emit geometryChanged();
}


void ViewItem::setViewRect(qreal x, qreal y, qreal width, qreal height) {
  setViewRect(QRectF(x, y, width, height));
}


QSize ViewItem::sizeOfGrip() const {
  return QSize(10,10);
}


QPainterPath ViewItem::topLeftGrip() const {
  if (_mouseMode == Default || _mouseMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  QPainterPath path;
  if (_mouseMode == Resize || _mouseMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::topRightGrip() const {
  if (_mouseMode == Default || _mouseMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPoint(sizeOfGrip().width(), 0), sizeOfGrip());
  QPainterPath path;
  if (_mouseMode == Resize || _mouseMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::bottomRightGrip() const {
  if (_mouseMode == Default || _mouseMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomRight() - QPoint(sizeOfGrip().width(), sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_mouseMode == Resize || _mouseMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::bottomLeftGrip() const {
  if (_mouseMode == Default || _mouseMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPoint(0, sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_mouseMode == Resize || _mouseMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::topMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::rightMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPoint(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::bottomMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPoint(0, sizeOfGrip().height()), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::leftMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);
  return mapFromScene(path);
}


QPainterPath ViewItem::grips() const {

  if (_mouseMode == Default || _mouseMode == Move)
    return QPainterPath();

  QPainterPath grips;
  grips.addPath(topLeftGrip());
  grips.addPath(topRightGrip());
  grips.addPath(bottomRightGrip());
  grips.addPath(bottomLeftGrip());
  grips.addPath(topMidGrip());
  grips.addPath(rightMidGrip());
  grips.addPath(bottomMidGrip());
  grips.addPath(leftMidGrip());
  return grips;
}


ViewItem::ActiveGrip ViewItem::activeGrip() const {
  return _activeGrip;
}


void ViewItem::setActiveGrip(ActiveGrip grip) {
  _activeGrip = grip;
}


QRectF ViewItem::selectBoundingRect() const {
  return mapToScene(itemShape()).boundingRect();
}


QRectF ViewItem::gripBoundingRect() const {
  QRectF bound = selectBoundingRect();
  bound.setTopLeft(bound.topLeft() - QPoint(sizeOfGrip().width(), sizeOfGrip().height()));
  bound.setWidth(bound.width() + sizeOfGrip().width());
  bound.setHeight(bound.height() + sizeOfGrip().height());
  return bound;
}


QRectF ViewItem::boundingRect() const {
  if (!isSelected())
    return QGraphicsRectItem::boundingRect();

  QPolygonF gripBound = mapFromScene(gripBoundingRect());
  return QRectF(gripBound[0], gripBound[2]);
}


QPainterPath ViewItem::shape() const {
  if (!isSelected())
    return itemShape();

  QPainterPath selectPath;
  selectPath.addPolygon(mapFromScene(selectBoundingRect()));
  selectPath.addPath(grips());
  return selectPath;
}

QLineF ViewItem::originLine() const {
  QRectF r = selectBoundingRect();
  r.setBottom(r.bottom() - (r.height() / 2));
  QPolygonF polygon = mapFromScene(r);
  QPointF right = polygon[2]; //bottomRight
  return QLineF(rect().center(), right);
}

void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  painter->save();
  painter->setPen(Qt::DotLine);
  if (isSelected()) {
    painter->drawPath(shape());
    if (_mouseMode == Resize)
      painter->fillPath(grips(), Qt::blue);
    else if (_mouseMode == Scale)
      painter->fillPath(grips(), Qt::black);
    else if (_mouseMode == Rotate)
      painter->fillPath(grips(), Qt::red);
  }

#ifdef DEBUG_GEOMETRY
//  painter->fillRect(selectBoundingRect(), Qt::blue);
  QColor semiRed(QColor(255, 0, 0, 50));
  painter->fillPath(shape(), semiRed);

  QPen p = painter->pen();
  painter->setPen(Qt::black);
  painter->drawLine(originLine());

  painter->setPen(Qt::white);
  painter->drawLine(_normalLine);

  painter->setPen(Qt::red);
  painter->drawLine(_rotationLine);
  painter->setPen(p);

  painter->drawText(rect().topLeft(), "TL");
  painter->drawText(rect().topRight(), "TR");
  painter->drawText(rect().bottomLeft(), "BL");
  painter->drawText(rect().bottomRight(), "BR");
#endif
  painter->restore();

  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(pen());
  painter->setBrush(brush());
  paint(painter); //this is the overload that subclasses should use...

//   QGraphicsRectItem::paint(painter, option, widget);
}


void ViewItem::paint(QPainter *painter) {
  painter->drawRect(rect());
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


void ViewItem::edit() {

  QList<ViewItem*> list;
  QList<QGraphicsItem*> select = scene()->selectedItems();
  foreach(QGraphicsItem *item, select) {
    list << qgraphicsitem_cast<ViewItem*>(item);
  }

  ViewItemDialog::self()->show(list);
}


void ViewItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setViewRect(poly.first().x(), poly.first().y(),
            poly.last().x() - poly.first().x(), poly.last().y() - poly.first().y());
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    setViewRect(rect().x(), rect().y(),
            poly.last().x() - rect().x(), poly.last().y() - rect().y());
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    setViewRect(rect().x(), rect().y(),
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

  QAction *editAction = menu.addAction(tr("Edit"));
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

  QAction *raiseAction = menu.addAction(tr("Raise"));
  connect(raiseAction, SIGNAL(triggered()), this, SLOT(raise()));

  QAction *lowerAction = menu.addAction(tr("Lower"));
  connect(lowerAction, SIGNAL(triggered()), this, SLOT(lower()));

  QAction *removeAction = menu.addAction(tr("Remove"));
  connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  menu.exec(event->screenPos());
}


void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (qgraphicsitem_cast<ViewItem*>(parentItem())) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() == View::Default) {
    if (mouseMode() == ViewItem::Default ||
        mouseMode() == ViewItem::Move ||
        activeGrip() == NoGrip) {
      parentView()->setMouseMode(View::Move);
      parentView()->undoStack()->beginMacro(tr("Move"));
    } else if (mouseMode() == ViewItem::Resize) {
      parentView()->setMouseMode(View::Resize);
      parentView()->undoStack()->beginMacro(tr("Resize"));
    } else if (mouseMode() == ViewItem::Scale) {
      parentView()->setMouseMode(View::Scale);
      parentView()->undoStack()->beginMacro(tr("Scale"));
    } else if (mouseMode() == ViewItem::Rotate) {
      parentView()->setMouseMode(View::Rotate);
      parentView()->undoStack()->beginMacro(tr("Rotate"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  QPointF l = event->lastPos();
  QPointF s = event->scenePos();

  if (mouseMode() == ViewItem::Rotate) {

    rotateTowards(l, p);

  } else if (mouseMode() == ViewItem::Resize || mouseMode() == ViewItem::Scale) {

    switch(_activeGrip) {
    case TopLeftGrip:
        setTopLeft(s); break;
    case TopRightGrip:
        setTopRight(s); break;
    case BottomRightGrip:
        setBottomRight(s); break;
    case BottomLeftGrip:
        setBottomLeft(s); break;
    case TopMidGrip:
        setTop(s); break;
    case RightMidGrip:
        setRight(s); break;
    case BottomMidGrip:
        setBottom(s); break;
    case LeftMidGrip:
        setLeft(s); break;
    case NoGrip:
      break;
    }

  }
}


void ViewItem::setTopLeft(const QPointF &point) {
//   qDebug() << "setTopLeft" << point << endl;

  QPointF anchor = selectTransform().map(rect().bottomRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopLeft(point);
  from.moveBottomRight(anchor);
  to.moveBottomRight(anchor);

  if (_mouseMode == Scale) {
    transformToRect(from, to);
  } else if (_mouseMode == Resize) {
    //FIXME;
  }
}


void ViewItem::setTopRight(const QPointF &point) {
//   qDebug() << "setTopRight" << point << endl;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopRight(point);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomLeft(const QPointF &point) {
//   qDebug() << "setBottomLeft" << point << endl;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomLeft(point);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomRight(const QPointF &point) {
//   qDebug() << "setBottomRight" << point << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomRight(point);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setTop(const QPointF &point) {
//   qDebug() << "setTop" << point << endl;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTop(point.y());
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottom(const QPointF &point) {
//   qDebug() << "setBottom" << point << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottom(point.y());
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setLeft(const QPointF &point) {
//   qDebug() << "setLeft" << point << endl;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setLeft(point.x());
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setRight(const QPointF &point) {
//   qDebug() << "setRight" << point << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setRight(point.x());
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


QTransform ViewItem::selectTransform() const {

  /* Converts a point on the rect() to a point on the selectBoundingRect()
     or the inverse by using selectTransform().inverted()...
  */

  QRectF from = rect();
  QRectF to = selectBoundingRect();
  QTransform rt = _rotationTransform.inverted(); //inverse rotation so far

  QPolygonF from_ = QPolygonF(rt.map(from));
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(mapFromScene(to));
  to_.pop_back(); //get rid of last closed point

  QTransform select;
  QTransform::quadToQuad(from_, to_, select);

  return _rotationTransform.inverted() * select * transform();
}


bool ViewItem::transformToRect(const QRectF &from, const QRectF &to) {
  //Not sure how to handle yet
  if (!to.isValid()) {
    return false;
  }

//   qDebug() << "Mapping from " << from << "to" << to << endl;

  QPolygonF from_(from);
  from_.pop_back(); //get rid of last closed point
  QPolygonF to_(to);
  to_.pop_back(); //get rid of last closed point
  return transformToRect(from_, to_);
}

bool ViewItem::transformToRect(const QPolygonF &from, const QPolygonF &to) {

  QTransform t;
  bool success = QTransform::quadToQuad(from, to, t);

  t = transform() * t;

  if (success) setTransform(t, false);
  return success;
}


void ViewItem::rotateTowards(const QPointF &corner, const QPointF &point) {

  QPointF origin = rect().center();
  _normalLine = QLineF(origin, corner);
  _rotationLine = QLineF(origin, point);

  qreal angle;

  if (mapToScene(point).y() >= mapToScene(origin).y()) {
/*    qDebug() << "positive" << endl;*/
    angle = originLine().angle(_rotationLine) - originLine().angle(_normalLine);
  } else {
/*    qDebug() << "negative" << endl;*/
    angle =  originLine().angle(_normalLine) - originLine().angle(_rotationLine);
  }

  QTransform t;
  t.translate(origin.x(), origin.y());
  t.rotate(angle);
  t.translate(-origin.x(), -origin.y());

  _rotationTransform = t * _rotationTransform;

  setTransform(t, true);
}


void ViewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {

  if (qgraphicsitem_cast<ViewItem*>(parentItem())) {
    event->ignore();
    return;
  }

  edit();
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  if (qgraphicsitem_cast<ViewItem*>(parentItem())) {
    event->ignore();
    return;
  }

  QPointF p = event->pos();
  if (topLeftGrip().contains(p)) {
    setActiveGrip(TopLeftGrip);
  } else if (topRightGrip().contains(p)) {
    setActiveGrip(TopRightGrip);
  } else if (bottomRightGrip().contains(p)) {
    setActiveGrip(BottomRightGrip);
  } else if (bottomLeftGrip().contains(p)) {
    setActiveGrip(BottomLeftGrip);
  } else if (topMidGrip().contains(p)) {
    setActiveGrip(TopMidGrip);
  } else if (rightMidGrip().contains(p)) {
    setActiveGrip(RightMidGrip);
  } else if (bottomMidGrip().contains(p)) {
    setActiveGrip(BottomMidGrip);
  } else if (leftMidGrip().contains(p)) {
    setActiveGrip(LeftMidGrip);
  } else {
    setActiveGrip(NoGrip);
  }

  QGraphicsRectItem::mousePressEvent(event);
}


void ViewItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

  if (qgraphicsitem_cast<ViewItem*>(parentItem())) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() != View::Default) {
    parentView()->setMouseMode(View::Default);
    parentView()->undoStack()->endMacro();
  } else if (!grips().contains(event->pos()) && event->button() & Qt::LeftButton) {
    switch (_mouseMode) {
    case Default:
    case Move:
    case Rotate:
      setMouseMode(Resize);
      break;
    case Resize:
      setMouseMode(Scale);
      break;
    case Scale:
      setMouseMode(Rotate);
      break;
    default:
      break;
    }
  }

  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
}


QVariant ViewItem::itemChange(GraphicsItemChange change, const QVariant &value) {

  if (change == ItemSelectedChange) {
    bool selected = value.toBool();
    if (!selected) {
      setMouseMode(ViewItem::Default);
      update();
    }
  }

  return QGraphicsItem::itemChange(change, value);
}


void ViewItem::viewMouseModeChanged(View::MouseMode oldMode) {
  if (parentView()->mouseMode() == View::Move) {
    _originalPosition = pos();
  } else if (parentView()->mouseMode() == View::Resize ||
             parentView()->mouseMode() == View::Scale ||
             parentView()->mouseMode() == View::Rotate) {
    _originalTransform = transform();
  } else if (oldMode == View::Move) {

    setPos(parentView()->snapPoint(pos()));

    new MoveCommand(this, _originalPosition, pos());
  } else if (oldMode == View::Resize) {
    new ResizeCommand(this, _originalTransform, transform());
  } else if (oldMode == View::Scale) {
    //new ScaleCommand(this, _originalTransform, transform());
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

// vim: ts=2 sw=2 et
