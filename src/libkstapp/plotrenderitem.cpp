/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "plotrenderitem.h"

#include <QTime>
#include <QMenu>
#include <QStatusBar>
#include <QMainWindow>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>

#include "plotitem.h"
#include "application.h"

// #define CURVE_DRAWING_TIME

namespace Kst {

PlotRenderItem::PlotRenderItem(PlotItem *parentItem)
  : ViewItem(parentItem->parentView()), _zoomRect(QRectF()) {

  setName(tr("Plot Render"));
  setParentItem(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAllowedGrips(0);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem->parentView(), SIGNAL(viewModeChanged(View::ViewMode)),
          this, SLOT(updateViewMode()));

  updateGeometry(); //the initial rect
  updateViewMode(); //the initial view
  createActions();
}


PlotRenderItem::~PlotRenderItem() {
}


PlotItem *PlotRenderItem::plotItem() const {
  return qobject_cast<PlotItem*>(qgraphicsitem_cast<ViewItem*>(parentItem()));
}


void PlotRenderItem::setType(RenderType type) {
  _type = type;
}


RenderType PlotRenderItem::type() {
  return _type;
}


QRectF PlotRenderItem::plotRect() const {
  QRectF plotRect = rect();
  plotRect = plotRect.normalized();
  plotRect.moveTopLeft(QPointF(0.0, 0.0));
  return plotRect;
}


QRectF PlotRenderItem::zoomRect() const {
  if (_zoomRect.isEmpty() || !_zoomRect.isValid())
    return projectionRect();
  else
    return _zoomRect;
}


QRectF PlotRenderItem::projectionRect() const {
  qreal minX, maxX, minY, maxY = 0.0;
  foreach (KstRelationPtr relation, relationList()) {
      minX = qMin(relation->minX(), minX);
      minY = qMin(relation->minY(), minY);
      maxX = qMax(relation->maxX(), maxX);
      maxY = qMax(relation->maxY(), maxY);
  }
  return QRectF(QPointF(minX, minY),
                QPointF(maxX, maxY));
}


void PlotRenderItem::setRelationList(const KstRelationList &relationList) {
  _relationList = relationList;
}


KstRelationList PlotRenderItem::relationList() const {
  return _relationList;
}


void PlotRenderItem::paint(QPainter *painter) {
  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->drawRect(rect());
  painter->setClipRect(rect());

#ifdef CURVE_DRAWING_TIME
  QTime time;
  time.start();
#endif

  paintRelations(painter);

  if (_selectionRect.isValid()) {
    painter->save();
    painter->setPen(Qt::black);
    painter->drawRect(_selectionRect.rect());
    painter->restore();
  }

#ifdef CURVE_DRAWING_TIME
  int elapsed = time.elapsed();
  qDebug()<<"curve drawing took" << elapsed << "to render.";
#endif
}


QString PlotRenderItem::leftLabel() const {
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty())
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::bottomLabel() const {
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty())
      return relation->xLabel();
  }
  return QString();
}


QString PlotRenderItem::rightLabel() const {
  //FIXME much less than ideal
  QString left = leftLabel();
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty() && relation->yLabel() != left)
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::topLabel() const {
  //FIXME much less than ideal
  QString bottom = bottomLabel();
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty() && relation->xLabel() != bottom)
      return relation->xLabel();
  }
  return QString();
}


void PlotRenderItem::createActions() {
  _zoomMaximum = new QAction(tr("Zoom Maximum"), this);
  _zoomMaximum->setShortcut(Qt::Key_M);
  plotItem()->parentView()->registerShortcut(_zoomMaximum);
  connect(_zoomMaximum, SIGNAL(triggered()), this, SLOT(zoomMaximum()));

  _zoomMaxSpikeInsensitive = new QAction(tr("Zoom Max Spike Insensitive"), this);
  _zoomMaxSpikeInsensitive->setShortcut(Qt::Key_S);

//   _zoomPrevious = new QAction(tr("Zoom Previous"), this);
//   _zoomPrevious->setShortcut(Qt::Key_R);

  _zoomYMeanCentered = new QAction(tr("Y-Zoom Mean-centered"), this);
  _zoomYMeanCentered->setShortcut(Qt::Key_A);

  _zoomXMaximum = new QAction(tr("X-Zoom Maximum"), this);
  _zoomXMaximum->setShortcut(Qt::CTRL+Qt::Key_M);

  _zoomXOut = new QAction(tr("X-Zoom Out"), this);
  _zoomXOut->setShortcut(Qt::SHIFT+Qt::Key_Right);

  _zoomXIn = new QAction(tr("X-Zoom In"), this);
  _zoomXIn->setShortcut(Qt::SHIFT+Qt::Key_Left);

  _zoomNormalizeXtoY = new QAction(tr("Normalize X Axis to Y Axis"), this);
  _zoomNormalizeXtoY->setShortcut(Qt::Key_N);

  _zoomToggleLogX = new QAction(tr("Toggle Log X Axis"), this);
  _zoomToggleLogX->setShortcut(Qt::Key_G);

  _zoomYLocalMaximum = new QAction(tr("Y-Zoom Local Maximum"), this);
  _zoomYLocalMaximum->setShortcut(Qt::SHIFT+Qt::Key_L);

  _zoomYMaximum = new QAction(tr("Y-Zoom Maximum"), this);
  _zoomYMaximum->setShortcut(Qt::SHIFT+Qt::Key_M);

  _zoomYOut = new QAction(tr("Y-Zoom Out"), this);
  _zoomYOut->setShortcut(Qt::SHIFT+Qt::Key_Up);

  _zoomYIn = new QAction(tr("Y-Zoom In"), this);
  _zoomYIn->setShortcut(Qt::SHIFT+Qt::Key_Down);

  _zoomNormalizeYtoX = new QAction(tr("Normalize Y Axis to X Axis"), this);
  _zoomNormalizeYtoX->setShortcut(Qt::SHIFT+Qt::Key_N);

  _zoomToggleLogY = new QAction(tr("Toggle Log Y Axis"), this);
  _zoomToggleLogY->setShortcut(Qt::Key_L);
}


void PlotRenderItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  addTitle(&menu);

  QAction *editAction = menu.addAction(tr("Edit"));
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

  QMenu zoom;
  zoom.setTitle(tr("Zoom"));

  zoom.addAction(_zoomMaximum);
  zoom.addAction(_zoomMaxSpikeInsensitive);
//   zoom.addAction(_zoomPrevious);
  zoom.addAction(_zoomYMeanCentered);

  zoom.addSeparator();

  zoom.addAction(_zoomXMaximum);
  zoom.addAction(_zoomXOut);
  zoom.addAction(_zoomXIn);
  zoom.addAction(_zoomNormalizeXtoY);
  zoom.addAction(_zoomToggleLogX);

  zoom.addSeparator();

  zoom.addAction(_zoomYLocalMaximum);
  zoom.addAction(_zoomYMaximum);
  zoom.addAction(_zoomYOut);
  zoom.addAction(_zoomYIn);
  zoom.addAction(_zoomNormalizeYtoX);
  zoom.addAction(_zoomToggleLogY);

  menu.addMenu(&zoom);

  menu.exec(event->screenPos());
}


void PlotRenderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  _selectionRect.setTo(event->pos());
  if (_selectionRect.isValid()) {
    update(); //FIXME should optimize instead of redrawing entire curve?
  }
}


void PlotRenderItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  _selectionRect.setFrom(event->pos());
}


void PlotRenderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  _zoomRect = mapToProjection(_selectionRect.rect());
  _selectionRect.reset();
  update();
}


void PlotRenderItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverMoveEvent(event);

  const QPointF p = mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(p.x())).arg(QString::number(p.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverEnterEvent(event);

  const QPointF p = mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(p.x())).arg(QString::number(p.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverLeaveEvent(event);

  kstApp->mainWindow()->statusBar()->showMessage(QString());
}


QTransform PlotRenderItem::projectionTransform() const {
  QTransform t;

  QRectF v = QRectF(rect().bottomLeft(), viewRect().topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(zoomRect());
  to_.pop_back(); //get rid of last closed point

  QTransform::quadToQuad(from_, to_, t);
  return t;
}


QPointF PlotRenderItem::mapToProjection(const QPointF &point) const {
  return projectionTransform().map(point);
}


QPointF PlotRenderItem::mapFromProjection(const QPointF &point) const {
  return projectionTransform().inverted().map(point);
}


QRectF PlotRenderItem::mapToProjection(const QRectF &rect) const {
  return projectionTransform().mapRect(rect);
}


QRectF PlotRenderItem::mapFromProjection(const QRectF &rect) const {
  return projectionTransform().inverted().mapRect(rect);
}


void PlotRenderItem::updateGeometry() {
  QRectF rect = plotItem()->rect().normalized();
  QPointF margin(plotItem()->marginWidth(), plotItem()->marginHeight());
  QPointF topLeft(rect.topLeft() + margin);
  QPointF bottomRight(rect.bottomRight() - margin);
  setViewRect(QRectF(topLeft, bottomRight));
}


void PlotRenderItem::updateViewMode() {
  switch (parentView()->viewMode()) {
  case View::Data:
    setCursor(Qt::CrossCursor);
    break;
  case View::Layout:
    setCursor(Qt::ArrowCursor);
    break;
  default:
    break;
  }
}


void PlotRenderItem::zoomMaximum() {
  qDebug() << "Zoom Maximum" << endl;
}


void PlotRenderItem::zoomMaxSpikeInsensitive() {
}


// void PlotRenderItem::zoomPrevious() {
// }


void PlotRenderItem::zoomYMeanCentered() {
}


void PlotRenderItem::zoomXMaximum() {
}


void PlotRenderItem::zoomXOut() {
}


void PlotRenderItem::zoomXIn() {
}


void PlotRenderItem::zoomNormalizeXtoY() {
}


void PlotRenderItem::zoomToggleLogX() {
}


void PlotRenderItem::zoomYLocalMaximum() {
}


void PlotRenderItem::zoomYMaximum() {
}


void PlotRenderItem::zoomYOut() {
}


void PlotRenderItem::zoomYIn() {
}


void PlotRenderItem::zoomNormalizeYtoX() {
}


void PlotRenderItem::zoomToggleLogY() {
}

}

// vim: ts=2 sw=2 et
