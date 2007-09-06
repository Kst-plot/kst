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
#include "viewgridlayout.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent),
    _mouseMode(Default),
    _lockAspectRatio(false),
    _layout(0),
    _activeGrip(NoGrip) {

  setName("ViewItem");
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
  //disconnect previous layout...
  if (_layout) {
    disconnect(this, SIGNAL(geometryChanged()), _layout, SLOT(update()));
  }

  _layout = layout;

  if (_layout) {
    connect(this, SIGNAL(geometryChanged()), _layout, SLOT(update()));
  }

  setHandlesChildEvents(_layout);
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

//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
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

//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
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

//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
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

//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
}


QPainterPath ViewItem::topMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);
//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
}


QPainterPath ViewItem::rightMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPoint(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);
//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
}


QPainterPath ViewItem::bottomMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPoint(0, sizeOfGrip().height()), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);
//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
}


QPainterPath ViewItem::leftMidGrip() const {
  if (_mouseMode == Default || _mouseMode == Move || _mouseMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);
//   if (_mouseMode != Resize)
    return mapFromScene(path);
//   else
//     return path;
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
  QRectF bound = /*_mouseMode != Resize ?*/ selectBoundingRect() /*: rect()*/;
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

//   if (_mouseMode != Resize)
    selectPath.addPolygon(mapFromScene(selectBoundingRect()));
//   else
//     selectPath.addPolygon(rect());

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
  Q_UNUSED(painter);
}


void ViewItem::edit() {
  ViewItemDialog::self()->show(this);
}


void ViewItem::createLayout() {
  LayoutCommand *layout = new LayoutCommand(this);
  layout->redo();
}


void ViewItem::breakLayout() {
  BreakLayoutCommand *layout = new BreakLayoutCommand(this);
  layout->redo();
}


void ViewItem::raise() {
  RaiseCommand *up = new RaiseCommand(this);
  up->redo();
}


void ViewItem::lower() {
  LowerCommand *down = new LowerCommand(this);
  down->redo();
}


void ViewItem::remove() {
  RemoveCommand *remove = new RemoveCommand(this);
  remove->redo();
}


void ViewItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(0, 0, 0, 0);
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

  QAction *layoutAction = menu.addAction(tr("Create Layout"));
  connect(layoutAction, SIGNAL(triggered()), this, SLOT(createLayout()));

  QAction *breakLayoutAction = menu.addAction(tr("Break Layout"));
  connect(breakLayoutAction, SIGNAL(triggered()), this, SLOT(breakLayout()));

  QAction *removeAction = menu.addAction(tr("Remove"));
  connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  menu.exec(event->screenPos());
}


void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
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

  } else if (mouseMode() == ViewItem::Resize) {

    switch(_activeGrip) {
    case TopLeftGrip:
        resizeTopLeft(p - l); break;
    case TopRightGrip:
        resizeTopRight(p - l); break;
    case BottomRightGrip:
        resizeBottomRight(p - l); break;
    case BottomLeftGrip:
        resizeBottomLeft(p - l); break;
    case TopMidGrip:
        resizeTop(p.y() - l.y()); break;
    case RightMidGrip:
        resizeRight(p.x() - l.x()); break;
    case BottomMidGrip:
        resizeBottom(p.y() - l.y()); break;
    case LeftMidGrip:
        resizeLeft(p.x() - l.x()); break;
    case NoGrip:
      break;
    }

  } else if (mouseMode() == ViewItem::Scale) {

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
        setTop(s.y()); break;
    case RightMidGrip:
        setRight(s.x()); break;
    case BottomMidGrip:
        setBottom(s.y()); break;
    case LeftMidGrip:
        setLeft(s.x()); break;
    case NoGrip:
      break;
    }

  }
}


void ViewItem::resizeTopLeft(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, false) : offset;
  r.setTopLeft(r.topLeft() + o);

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeTopRight(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, true) : offset;
  r.setTopRight(r.topRight() + o);

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeBottomLeft(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, true) : offset;
  r.setBottomLeft(r.bottomLeft() + o);

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeBottomRight(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, false) : offset;
  r.setBottomRight(r.bottomRight() + o);

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeTop(qreal offset) {
  QRectF r = rect();
  r.setTop(r.top() + offset);
  setViewRect(r);
}


void ViewItem::resizeBottom(qreal offset) {
  QRectF r = rect();
  r.setBottom(r.bottom() + offset);
  setViewRect(r);
}


void ViewItem::resizeLeft(qreal offset) {
  QRectF r = rect();
  r.setLeft(r.left() + offset);
  setViewRect(r);
}


void ViewItem::resizeRight(qreal offset) {
  QRectF r = rect();
  r.setRight(r.right() + offset);
  setViewRect(r);
}


void ViewItem::setTopLeft(const QPointF &point) {
//   qDebug() << "setTopLeft" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().bottomRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopLeft(p);
  from.moveBottomRight(anchor);
  to.moveBottomRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setTopRight(const QPointF &point) {
//   qDebug() << "setTopRight" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopRight(p);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomLeft(const QPointF &point) {
//   qDebug() << "setBottomLeft" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomLeft(p);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomRight(const QPointF &point) {
//   qDebug() << "setBottomRight" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomRight(p);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setTop(qreal y) {
//   qDebug() << "setTop" << x << endl;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTop(y);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottom(qreal y) {
//   qDebug() << "setBottom" << x << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottom(y);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setLeft(qreal x) {
//   qDebug() << "setLeft" << x << endl;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setLeft(x);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setRight(qreal x) {
//   qDebug() << "setRight" << x << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setRight(x);
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


QPointF ViewItem::lockOffset(const QPointF &offset, qreal ratio, bool oddCorner) const {
  qreal x;
  qreal y;

  if (offset.x() < 0 && offset.y() > 0) {
    x = offset.x();
    y = x == 0 ? 0 : (1 / ratio) * x;
  } else if (offset.y() < 0 && offset.x() > 0) {
    y = offset.y();
    x = y == 0 ? 0 : ratio * y;
  } else if (qAbs(offset.x()) < qAbs(offset.y())) {
    x = offset.x();
    y = x == 0 ? 0 : (1 / ratio) * x;
  } else {
    y = offset.y();
    x = y == 0 ? 0 : ratio * y;
  }

  QPointF o = offset;
  if (oddCorner) {
    o = QPointF(y == offset.y() ? -x : x,
                x == offset.x() ? -y : y);
  } else {
    o = QPointF(x, y);
  }

//   qDebug() << "lockOffset"
//             << "ratio:" << ratio
//             << "offset:" << offset
//             << "o:" << o
//             << endl;

  return o;
}


void ViewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
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

  if (parentView()->viewMode() == View::Data) {
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
    _originalRect = rect();
    _originalTransform = transform();
  } else if (oldMode == View::Move && _originalPosition != pos()) {
    setPos(parentView()->snapPoint(pos()));
    new MoveCommand(this, _originalPosition, pos());

    maybeReparent();
  } else if (oldMode == View::Resize && _originalRect != rect()) {
    new ResizeCommand(this, _originalRect, rect());

    maybeReparent();
  } else if (oldMode == View::Scale && _originalTransform != transform()) {
    new ScaleCommand(this, _originalTransform, transform());

    maybeReparent();
  } else if (oldMode == View::Rotate && _originalTransform != transform()) {
    new RotateCommand(this, _originalTransform, transform());

    maybeReparent();
  }
}


bool ViewItem::maybeReparent() {
  //First get a list of all items that collide with this one
  QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemShape);

  bool topLevel = !parentItem();
  QPointF scenePos = topLevel ? pos() : parentItem()->mapToScene(pos());

#ifdef DEBUG_REPARENT
  qDebug() << "maybeReparent" << this
           << "topLevel:" << (topLevel ? "true" : "false")
           << "scenePos:" << scenePos
           << endl;
#endif

  //Doesn't collide then reparent to top-level
  if (collisions.isEmpty() && !topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;
#endif
    setParentItem(0);
    setPos(scenePos);
    return true;
  }

  //Look for collisions that completely contain us
  foreach (QGraphicsItem *item, collisions) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);

    if (!viewItem || viewItem->layout() /*don't break existing layouts*/)
      continue;

    if (viewItem->collidesWithItem(this, Qt::ContainsItemShape)) {

      if (parentItem() == viewItem) /*already done*/
        return false;

#ifdef DEBUG_REPARENT
      qDebug() << "reparent to" << viewItem << endl;
#endif
      setParentItem(viewItem);
      setPos(viewItem->mapFromScene(scenePos));
      return true;
    }
  }

  //No suitable collisions then reparent to top-level
  if (!topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;
#endif
    setParentItem(0);
    setPos(scenePos);
    return true;
  }

  return false;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, ViewItem *viewItem) {
    dbg.nospace() << viewItem->name();
    return dbg.space();
}
#endif

ViewItemCommand::ViewItemCommand(const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->tabWidget()->currentView()->currentViewItem()) {
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


void CreateLayoutBoxCommand::createItem() {
  _item = new ViewItem(_view);
  _view->scene()->addItem(_item);
  _item->setZValue(1);
  _item->setPos(_view->sceneRect().topLeft());
  _item->setViewRect(_view->sceneRect());

  QList<QGraphicsItem*> list = _view->items();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (!viewItem || viewItem->parentItem() || !viewItem->isVisible() || viewItem == _item)
      continue;

    viewItem->setParentItem(_item);
  }
  _item->createLayout();
  _view->setLayoutBoxItem(_item);
}


void LayoutCommand::undo() {
  ViewGridLayout *layout = _item->layout();
  if (!layout)
    return;

  _item->setLayout(0);
  delete layout;
}


void LayoutCommand::redo() {
  ViewGridLayout *layout = new ViewGridLayout(_item);

  int column = 0;

  QList<QGraphicsItem*> list = _item->QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (!viewItem)
      continue;
    layout->addViewItem(viewItem, 0, column++);
  }
  layout->update();
}


void BreakLayoutCommand::undo() {
  ViewGridLayout *layout = new ViewGridLayout(_item);

  int column = 0;

  QList<QGraphicsItem*> list = _item->QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (!viewItem)
      continue;
    layout->addViewItem(viewItem, 0, column++);
  }
  layout->update();
}


void BreakLayoutCommand::redo() {
  ViewGridLayout *layout = _item->layout();
  if (!layout)
    return;

  _item->setLayout(0);
  delete layout;
}


void MoveCommand::undo() {
  _item->setPos(_originalPos);
}


void MoveCommand::redo() {
  _item->setPos(_newPos);
}


void ResizeCommand::undo() {
  _item->setViewRect(_originalRect);
}


void ResizeCommand::redo() {
  _item->setViewRect(_newRect);
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
