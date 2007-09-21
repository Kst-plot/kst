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
#include "application.h"
#include "tabwidget.h"
#include "viewitemdialog.h"
#include "viewgridlayout.h"

#include "layoutboxitem.h"

#include "gridlayouthelper.h"

#include <math.h>

#include <QMenu>
#include <QDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSceneContextMenuEvent>

static const double ONE_PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * ONE_PI;
static double RAD2DEG = 180.0 / ONE_PI;

// #define DEBUG_GEOMETRY
// #define DEBUG_REPARENT
#define INKSCAPE_MODE 0

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent),
    _gripMode(Move),
    _allowedGripModes(Move | Resize | Rotate /*| Scale*/),
    _hovering(false),
    _lockAspectRatio(false),
    _layout(0),
    _activeGrip(NoGrip),
    _allowedGrips(TopLeftGrip | TopRightGrip | BottomRightGrip | BottomLeftGrip |
                  TopMidGrip | RightMidGrip | BottomMidGrip | LeftMidGrip) {

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


ViewItem::GripMode ViewItem::gripMode() const {
  return _gripMode;
}


void ViewItem::setGripMode(GripMode mode) {
  _gripMode = mode;
  update();
}


ViewItem::GripModes ViewItem::allowedGripModes() const {
  return _allowedGripModes;
}


void ViewItem::setAllowedGripModes(GripModes modes) {
  _allowedGripModes = modes;
}


bool ViewItem::isAllowed(GripMode mode) const {
  return _allowedGripModes & mode;
}


ViewGridLayout *ViewItem::layout() const {
  return _layout;
}


void ViewItem::setLayout(ViewGridLayout *layout) {
  if (_layout == layout)
    return;

  //disconnect previous layout...
  if (_layout) {
    _layout->setEnabled(false);
    disconnect(this, SIGNAL(geometryChanged()), _layout, SLOT(update()));
  }

  _layout = layout;

  if (_layout) {
    _layout->setEnabled(true);
    connect(this, SIGNAL(geometryChanged()), _layout, SLOT(update()));
  }

  setHandlesChildEvents(_layout);
}


QRectF ViewItem::viewRect() const {
  return rect();
}


void ViewItem::setViewRect(const QRectF &viewRect) {
  QRectF oldViewRect = rect();

  if (oldViewRect == viewRect)
    return;

  setRect(viewRect);
  emit geometryChanged();

  if (layout())
    return;

  foreach (QGraphicsItem *item, QGraphicsItem::children()) {
    if (item->parentItem() != this)
      continue;

    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);

    if (!viewItem)
      continue;

    ViewItem::updateChildGeometry(viewItem, oldViewRect, viewRect);
  }
}


void ViewItem::setViewRect(qreal x, qreal y, qreal width, qreal height) {
  setViewRect(QRectF(x, y, width, height));
}


QSizeF ViewItem::sizeOfGrip() const {
  if (!parentView())
    return QSizeF();

  int base = 15;
#if INKSCAPE_MODE
  return mapFromScene(parentView()->mapToScene(QRect(0, 0, base, base)).boundingRect()).boundingRect().size();
#else
  return parentView()->mapToScene(QRect(0, 0, base, base)).boundingRect().size();
#endif
}


QPainterPath ViewItem::topLeftGrip() const {
  if (_gripMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::topRightGrip() const {
  if (_gripMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomRightGrip() const {
  if (_gripMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomRight() - QPointF(sizeOfGrip().width(), sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomLeftGrip() const {
  if (_gripMode == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::topMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::rightMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::leftMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::grips() const {

  if (_gripMode == Move)
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


ViewItem::ActiveGrips ViewItem::allowedGrips() const {
  return _allowedGrips;
}


void ViewItem::setAllowedGrips(ActiveGrips grips) {
  _allowedGrips = grips;
}


bool ViewItem::isAllowed(ActiveGrip grip) const {
  return _allowedGrips & grip;
}


QRectF ViewItem::selectBoundingRect() const {
#if INKSCAPE_MODE
  return mapToScene(itemShape()).boundingRect();
#else
  return rect();
#endif
}


QRectF ViewItem::gripBoundingRect() const {
  QRectF bound = selectBoundingRect();
  bound.setTopLeft(bound.topLeft() - QPointF(sizeOfGrip().width() / 2.0, sizeOfGrip().height() / 2.0));
  bound.setWidth(bound.width() + sizeOfGrip().width() / 2.0);
  bound.setHeight(bound.height() + sizeOfGrip().height() / 2.0);
  return bound;
}


QRectF ViewItem::boundingRect() const {
  bool inCreation = false;
  if (parentView()) /* false when exiting */
    inCreation = parentView()->mouseMode() == View::Create;
  if (!isSelected() && !isHovering() || inCreation)
    return QGraphicsRectItem::boundingRect();

#if INKSCAPE_MODE
  QPolygonF gripBound = mapFromScene(gripBoundingRect());
#else
  QPolygonF gripBound = gripBoundingRect();
#endif
  return QRectF(gripBound[0], gripBound[2]);
}


QPainterPath ViewItem::shape() const {
  if (!isSelected() && !isHovering() || parentView()->mouseMode() == View::Create)
    return itemShape();

  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);

#if INKSCAPE_MODE
    selectPath.addPolygon(mapFromScene(selectBoundingRect()));
#else
    selectPath.addPolygon(rect());
#endif

  selectPath.addPath(grips());
  return selectPath;
}


void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(pen());
  painter->setBrush(brush());
  paint(painter); //this is the overload that subclasses should use...

  painter->save();
  painter->setPen(Qt::DotLine);
  painter->setBrush(Qt::NoBrush);
  if (isSelected() || isHovering() && parentView()->mouseMode() != View::Create) {
    painter->drawPath(shape());
    if (_gripMode == Resize)
      painter->fillPath(grips(), Qt::blue);
    else if (_gripMode == Scale)
      painter->fillPath(grips(), Qt::black);
    else if (_gripMode == Rotate)
      painter->fillPath(grips(), Qt::red);
  }

#ifdef DEBUG_GEOMETRY
//  painter->fillRect(selectBoundingRect(), Qt::blue);
  QColor semiRed(QColor(255, 0, 0, 50));
  painter->fillPath(shape(), semiRed);

  QPen p = painter->pen();

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

//   QGraphicsRectItem::paint(painter, option, widget);
}


void ViewItem::paint(QPainter *painter) {
  Q_UNUSED(painter);
}


void ViewItem::edit() {
  ViewItemDialog editDialog(this);
  editDialog.exec();
}


void ViewItem::createLayout() {
  LayoutCommand *layout = new LayoutCommand(this);
  layout->createLayout();
}


void ViewItem::breakLayout() {
  if (!layout())
    return;

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
    setViewRect(0.0, 0.0, 0.0, 0.0);
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
    maybeReparent();
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

  QAction *layoutAction = menu.addAction(tr("Create layout"));
  connect(layoutAction, SIGNAL(triggered()), this, SLOT(createLayout()));
  layoutAction->setEnabled(!layout());

  QAction *breakLayoutAction = menu.addAction(tr("Break layout"));
  connect(breakLayoutAction, SIGNAL(triggered()), this, SLOT(breakLayout()));
  breakLayoutAction->setEnabled(layout());

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
    if (gripMode() == ViewItem::Move || activeGrip() == NoGrip) {
      parentView()->setMouseMode(View::Move);
      parentView()->undoStack()->beginMacro(tr("Move"));
    } else if (gripMode() == ViewItem::Resize) {
      parentView()->setMouseMode(View::Resize);
      parentView()->undoStack()->beginMacro(tr("Resize"));
    } else if (gripMode() == ViewItem::Scale) {
      parentView()->setMouseMode(View::Scale);
      parentView()->undoStack()->beginMacro(tr("Scale"));
    } else if (gripMode() == ViewItem::Rotate) {
      parentView()->setMouseMode(View::Rotate);
      parentView()->undoStack()->beginMacro(tr("Rotate"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  QPointF l = event->lastPos();
  QPointF s = event->scenePos();

  if (gripMode() == ViewItem::Rotate) {

#if INKSCAPE_MODE
    rotateTowards(l, p);
#else
    switch(_activeGrip) {
    case TopLeftGrip:
        rotateTowards(topLeftGrip().boundingRect().center(), p); break;
    case TopRightGrip:
        rotateTowards(topRightGrip().boundingRect().center(), p); break;
    case BottomRightGrip:
        rotateTowards(bottomRightGrip().boundingRect().center(), p); break;
    case BottomLeftGrip:
        rotateTowards(bottomLeftGrip().boundingRect().center(), p); break;
    case TopMidGrip:
        rotateTowards(topMidGrip().boundingRect().center(), p); break;
    case RightMidGrip:
        rotateTowards(rightMidGrip().boundingRect().center(), p); break;
    case BottomMidGrip:
        rotateTowards(bottomMidGrip().boundingRect().center(), p); break;
    case LeftMidGrip:
        rotateTowards(leftMidGrip().boundingRect().center(), p); break;
    case NoGrip:
      break;
    }
#endif

  } else if (gripMode() == ViewItem::Resize) {

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

  } else if (gripMode() == ViewItem::Scale) {

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

  QPointF origin = centerOfRotation();
  if (origin == corner || origin == point)
    return;

  _normalLine = QLineF(origin, corner);
  _rotationLine = QLineF(origin, point);

  qreal angle1 = ::acos(_normalLine.dx() / _normalLine.length());
  if (_normalLine.dy() >= 0)
      angle1 = TWO_PI - angle1;

  qreal angle2 = ::acos(_rotationLine.dx() / _rotationLine.length());
  if (_rotationLine.dy() >= 0)
      angle2 = TWO_PI - angle2;

  qreal angle = RAD2DEG * (angle1 - angle2);

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


bool ViewItem::maybeReparent() {
  //First get a list of all items that collide with this one
  QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemShape);

  bool topLevel = !parentItem();
  QPointF origin = mapToScene(QPointF(0,0));

#ifdef DEBUG_REPARENT
  qDebug() << "maybeReparent" << this
           << "topLevel:" << (topLevel ? "true" : "false")
           << "origin:" << origin

           << endl;
#endif

  //Doesn't collide then reparent to top-level
  if (collisions.isEmpty() && !topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //Look for collisions that completely contain us
  foreach (QGraphicsItem *item, collisions) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);

    if (!viewItem) /*bah*/
      continue;

    if (!viewItem->collidesWithItem(this, Qt::ContainsItemShape)) /*doesn't contain*/
      continue;

    if (parentItem() == viewItem) { /*already done*/
#ifdef DEBUG_REPARENT
      qDebug() << "already in containing parent" << endl;
#endif
      return false;
    }

    if (viewItem->layout()) /*don't crash existing layout*/
      continue;

#ifdef DEBUG_REPARENT
    qDebug() << "reparent to" << viewItem << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    if (!topLevel) /*bring the old parent's transform with us*/
      setTransform(parentItem()->transform(), true);

    /*cancel out the new parent's initial transform*/
    setTransform(viewItem->transform().inverted(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentItem(viewItem);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //No suitable collisions then reparent to top-level
  if (!topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  return false;
}


void ViewItem::updateChildGeometry(ViewItem *child, const QRectF &oldParentRect,
                                                    const QRectF &newParentRect) {
//  qDebug() << "ViewItem::updateChildGeometry" << oldParentRect << newParentRect << endl;

  qreal dx = oldParentRect.width() ? newParentRect.width() / oldParentRect.width() : 0.0;
  qreal dy = oldParentRect.height() ? newParentRect.height() / oldParentRect.height() : 0.0;

  bool topChanged = oldParentRect.top() != newParentRect.top();
  bool leftChanged = oldParentRect.left() != newParentRect.left();
  bool bottomChanged = oldParentRect.bottom() != newParentRect.bottom();
  bool rightChanged = oldParentRect.right() != newParentRect.right();

  //Lock aspect ratio for rotating objects.
  //Always pick the smaller resize factor so the bounds of the rotated object
  //do not extend outside of parent.

  //FIXME is the child rotated with respect to the parent is the real question...
  if (child->transform().isRotating() /*|| child->lockAspectRatio()*/) {
    dx = qMin(dx, dy);
    dy = dx;
  }

//   if (child->lockAspectRatio()) {
//     dx = qMax(dx, dy);
//     dy = dx;
//   }

  QRectF rect = child->rect();

  qreal width = rect.width() * dx;
  qreal height = rect.height() * dy;

  rect.setBottom(rect.top() + height);
  rect.setRight(rect.left() + width);

  if (topChanged) {
    QPointF offset = oldParentRect.bottomRight() - child->mapToParent(child->rect().bottomRight());

    qreal xOff = offset.x() * dx;
    qreal yOff = offset.y() * dy;

    QPointF newBottomRight = oldParentRect.bottomRight() - QPointF(xOff, yOff);

    QPointF o = child->pos() - child->mapToParent(rect.topLeft());

    QRectF r = rect;
    r.moveBottom(child->mapFromParent(newBottomRight).y());

    child->setPos(child->mapToParent(r.topLeft()) + o);
  }

  if (leftChanged) {
    QPointF offset = oldParentRect.bottomRight() - child->mapToParent(child->rect().bottomRight());

    qreal xOff = offset.x() * dx;
    qreal yOff = offset.y() * dy;

    QPointF newBottomRight = oldParentRect.bottomRight() - QPointF(xOff, yOff);

    QPointF o = child->pos() - child->mapToParent(rect.topLeft());

    QRectF r = rect;
    r.moveRight(child->mapFromParent(newBottomRight).x());

    child->setPos(child->mapToParent(r.topLeft()) + o);
  }

  if (bottomChanged) {

    QPointF offset = child->mapToParent(child->rect().topLeft()) - oldParentRect.topLeft();

    qreal xOff = offset.x() * dx;
    qreal yOff = offset.y() * dy;

    QPointF newTopLeft = oldParentRect.topLeft() + QPointF(xOff, yOff);

    QPointF o = child->pos() - child->mapToParent(rect.topLeft());

    QRectF r = rect;
    r.moveTop(child->mapFromParent(newTopLeft).y());

    child->setPos(child->mapToParent(r.topLeft()) + o);
  }

  if (rightChanged) {

    QPointF offset = child->mapToParent(child->rect().topLeft()) - oldParentRect.topLeft();

    qreal xOff = offset.x() * dx;
    qreal yOff = offset.y() * dy;

    QPointF newTopLeft = oldParentRect.topLeft() + QPointF(xOff, yOff);

    QPointF o = child->pos() - child->mapToParent(rect.topLeft());

    QRectF r = rect;
    r.moveLeft(child->mapFromParent(newTopLeft).x());

    child->setPos(child->mapToParent(r.topLeft()) + o);
  }

//   qDebug() << "resize"
//             << "\nbefore:" << child->rect()
//             << "\nafter:" << rect
//             << "\nwidth:" << width
//             << "\nheight:" << height
//             << endl;

  child->setViewRect(rect);
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
  if (isAllowed(TopLeftGrip) && topLeftGrip().contains(p)) {
    setActiveGrip(TopLeftGrip);
  } else if (isAllowed(TopRightGrip) && topRightGrip().contains(p)) {
    setActiveGrip(TopRightGrip);
  } else if (isAllowed(BottomRightGrip) && bottomRightGrip().contains(p)) {
    setActiveGrip(BottomRightGrip);
  } else if (isAllowed(BottomLeftGrip) && bottomLeftGrip().contains(p)) {
    setActiveGrip(BottomLeftGrip);
  } else if (isAllowed(TopMidGrip) && topMidGrip().contains(p)) {
    setActiveGrip(TopMidGrip);
  } else if (isAllowed(RightMidGrip) && rightMidGrip().contains(p)) {
    setActiveGrip(RightMidGrip);
  } else if (isAllowed(BottomMidGrip) && bottomMidGrip().contains(p)) {
    setActiveGrip(BottomMidGrip);
  } else if (isAllowed(LeftMidGrip) && leftMidGrip().contains(p)) {
    setActiveGrip(LeftMidGrip);
  } else {
    setActiveGrip(NoGrip);
  }

  if (!grips().contains(event->pos()) && event->button() & Qt::LeftButton) {
    setGripMode(nextGripMode(_gripMode));
  }

  QGraphicsRectItem::mousePressEvent(event);
}


ViewItem::GripMode ViewItem::nextGripMode(GripMode currentMode) const {
  if (!(_allowedGripModes & (Resize | Rotate | Scale)))
    return currentMode;

  switch (currentMode) {
  case Move:
    if (isAllowed(Resize))
      return Resize;
    else
      return nextGripMode(Resize);
    break;
  case Resize:
    if (isAllowed(Scale))
      return Scale;
    else
      return nextGripMode(Scale);
    break;
  case Scale:
    if (isAllowed(Rotate))
      return Rotate;
    else
      return nextGripMode(Rotate);
    break;
  case Rotate:
    if (isAllowed(Resize))
      return Resize;
    else
      return nextGripMode(Resize);
    break;
  default:
    break;
  }

  return currentMode;
}


void ViewItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() != View::Default) {
    parentView()->setMouseMode(View::Default);
    parentView()->undoStack()->endMacro();
  }

  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
}


void ViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  _hovering = true;
  update();
}


void ViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  _hovering = false;
  update();
}


QVariant ViewItem::itemChange(GraphicsItemChange change, const QVariant &value) {

  if (change == ItemSelectedChange) {
    bool selected = value.toBool();
    if (!selected) {
      setGripMode(ViewItem::Move);
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


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, ViewItem *viewItem) {
    dbg.nospace() << viewItem->name();
    return dbg.space();
}
#endif


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
  Q_ASSERT(_item);
  _item->hide();
}


void CreateCommand::redo() {
  Q_ASSERT(_item);
  _item->show();
}


void CreateCommand::createItem() {
  Q_ASSERT(_item);
  Q_ASSERT(_view);

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


void LayoutCommand::undo() {
  Q_ASSERT(_layout);
  _layout->reset();
  _item->setLayout(0);
}


void LayoutCommand::redo() {
  Q_ASSERT(_layout);
  _item->setLayout(_layout);
  _layout->update();
}


void LayoutCommand::createLayout() {
  Q_ASSERT(_item);
  Q_ASSERT(_item->parentView());

  QList<ViewItem*> viewItems;
  QList<QGraphicsItem*> list = _item->QGraphicsItem::children();
  if (list.isEmpty())
    return; //not added to undostack

  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem || viewItem->parentItem() != _item)
      continue;
    viewItems.append(viewItem);
  }

  if (viewItems.isEmpty())
    return; //not added to undostack

  Grid *grid = Grid::buildGrid(viewItems);
  Q_ASSERT(grid);

  _layout = new ViewGridLayout(_item);

  foreach (ViewItem *v, viewItems) {
    int r = 0, c = 0, rs = 0, cs = 0;
    if (grid->locateWidget(v, r, c, rs, cs)) {
      if (rs * cs == 1) {
        _layout->addViewItem(v, r, c, 1, 1);
      } else {
        _layout->addViewItem(v, r, c, rs, cs);
      }
    } else {
      qDebug() << "ooops, viewItem does not fit in layout" << endl;
    }
  }

  if (qobject_cast<LayoutBoxItem*>(_item)) {
    _layout->setMargin(QSizeF());
    _layout->setSpacing(QSizeF());
    QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                     _item, SLOT(setEnabled(bool)));
  }

  _layout->update();
  _item->parentView()->undoStack()->push(this);
}


void BreakLayoutCommand::undo() {
  Q_ASSERT(_layout);
  _item->setLayout(_layout);
  _layout->update();
}


void BreakLayoutCommand::redo() {
  _layout = _item->layout();
  Q_ASSERT(_layout);
  _item->setLayout(0);
}


void MoveCommand::undo() {
  Q_ASSERT(_item);
  _item->setPos(_originalPos);
}


void MoveCommand::redo() {
  Q_ASSERT(_item);
  _item->setPos(_newPos);
}


void ResizeCommand::undo() {
  Q_ASSERT(_item);
  _item->setViewRect(_originalRect);
}


void ResizeCommand::redo() {
  Q_ASSERT(_item);
  _item->setViewRect(_newRect);
}


void RemoveCommand::undo() {
  Q_ASSERT(_item);
  _item->show();
}


void RemoveCommand::redo() {
  Q_ASSERT(_item);
  _item->hide();
}


void RaiseCommand::undo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() - 1);
}


void RaiseCommand::redo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() + 1);
}


void LowerCommand::undo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() +1);
}


void LowerCommand::redo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() - 1);
}


void TransformCommand::undo() {
  Q_ASSERT(_item);
  _item->setTransform(_originalTransform);
}


void TransformCommand::redo() {
  Q_ASSERT(_item);
  _item->setTransform(_newTransform);
}

}

// vim: ts=2 sw=2 et
