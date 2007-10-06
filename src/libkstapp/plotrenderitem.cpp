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
  : ViewItem(parentItem->parentView()),
  _isTiedZoom(false),
  _xAxisZoomMode(Auto),
  _yAxisZoomMode(AutoBorder),
  _isXAxisLog(false),
  _isYAxisLog(false),
  _xLogBase(10.0),
  _yLogBase(10.0) {

  setName(tr("Plot Render"));
  setParentItem(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAllowedGrips(0);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(labelVisibilityChanged()),
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


RenderType PlotRenderItem::type() {
  return _type;
}


void PlotRenderItem::setType(RenderType type) {
  _type = type;
}


bool PlotRenderItem::isTiedZoom() const {
  return _isTiedZoom;
}


void PlotRenderItem::setTiedZoom(bool tiedZoom) {
  _isTiedZoom = tiedZoom;
}


PlotRenderItem::ZoomMode PlotRenderItem::xAxisZoomMode() const {
  return _xAxisZoomMode;
}


void PlotRenderItem::setXAxisZoomMode(ZoomMode mode) {
  _xAxisZoomMode = mode;
}


PlotRenderItem::ZoomMode PlotRenderItem::yAxisZoomMode() const {
  return _yAxisZoomMode;
}


void PlotRenderItem::setYAxisZoomMode(ZoomMode mode) {
  _yAxisZoomMode = mode;
}


bool PlotRenderItem::isXAxisLog() const {
  return _isXAxisLog;
}


void PlotRenderItem::setXAxisLog(bool log) {
  _isXAxisLog = log;
}


qreal PlotRenderItem::xLogBase() const {
  return _xLogBase;
}


void PlotRenderItem::setXLogBase(qreal xLogBase) {
  _xLogBase = xLogBase;
}


bool PlotRenderItem::isYAxisLog() const {
  return _isYAxisLog;
}


void PlotRenderItem::setYAxisLog(bool log) {
  _isYAxisLog = log;
}


qreal PlotRenderItem::yLogBase() const {
  return _yLogBase;
}


void PlotRenderItem::setYLogBase(qreal yLogBase) {
  _yLogBase = yLogBase;
}


QRectF PlotRenderItem::plotRect() const {
  QRectF plotRect = rect();
  plotRect = plotRect.normalized();
  plotRect.moveTopLeft(QPointF(0.0, 0.0));
  return plotRect;
}


QRectF PlotRenderItem::projectionRect() const {
    return _projectionRect;
}


void PlotRenderItem::setProjectionRect(const QRectF &rect) {
  if (_projectionRect == rect || rect.isEmpty() || !rect.isValid())
    return;

  qDebug() << "=== setProjectionRect() ======================>\n"
            << "computedProjectionRect" << computedProjectionRect() << "\n"
            << "before:" << _projectionRect << "\n"
            << "after:" << rect << endl;
  _projectionRect = rect;
  update();
}


QRectF PlotRenderItem::computedProjectionRect() const {
  qreal minX, minY, maxX, maxY;
  xAxisRange(&minX, &maxX);
  yAxisRange(&minY, &maxY);

  return QRectF(QPointF(minX, minY),
                QPointF(maxX, maxY));
}


void PlotRenderItem::xAxisRange(qreal *min, qreal *max) const {
  qreal minimum = isXAxisLog() ? 0.0 : -0.1;
  qreal maximum = 0.1;
  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      //If the axis is in log mode, the lower extent will be the
      //minimum value larger than zero.
      if (isXAxisLog())
        minimum = minimum <= 0.0 ? relation->minPosX() : qMin(relation->minPosX(), minimum);
      else
        minimum = qMin(relation->minX(), minimum);

      maximum = qMax(relation->maxX(), maximum);
  }

  switch (_xAxisZoomMode) {
  case Auto:
    break; //nothing more...
  case AutoBorder:
    if (isXAxisLog()) {
      minimum = log10(minimum)/log10(xLogBase());
      maximum = maximum > 0.0 ? log10(maximum) : 0.0;
      qreal dx = qAbs(maximum - minimum) * 0.025;
      maximum = pow(xLogBase(), maximum + dx);
      minimum = pow(xLogBase(), minimum - dx);
    } else {
      qreal dx = qAbs(maximum - minimum) * 0.025;
      maximum += dx;
      minimum -= dx;
    }
  case Expression:
    break; //FIXME limits are given by scalar equations
  case SpikeInsensitive:
    break; //FIXME auto with algorithm to detect spikes TBD
  case MeanCentered:
    break; //FIXME the mean of all active curves
  default:
    break;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::yAxisRange(qreal *min, qreal *max) const {
  qreal minimum = isYAxisLog() ? 0.0 : -0.1;
  qreal maximum = 0.1;
  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      //If the axis is in log mode, the lower extent will be the
      //minimum value larger than zero.
      if (isYAxisLog())
        minimum = minimum <= 0.0 ? relation->minPosY() : qMin(relation->minPosY(), minimum);
      else
        minimum = qMin(relation->minY(), minimum);

      maximum = qMax(relation->maxY(), maximum);
  }

  switch (_yAxisZoomMode) {
  case Auto:
    break; //nothing more...
  case AutoBorder:
    if (isYAxisLog()) {
      minimum = log10(minimum)/log10(yLogBase());
      maximum = maximum > 0.0 ? log10(maximum) : 0.0;
      qreal dy = qAbs(maximum - minimum) * 0.025;
      maximum = pow(yLogBase(), maximum + dy);
      minimum = pow(yLogBase(), minimum - dy);
    } else {
      qreal dy = qAbs(maximum - minimum) * 0.025;
      maximum += dy;
      minimum -= dy;
    }
    break; //auto mode, plus a 2.5% border on top and bottom.
  case Expression:
    break; //FIXME limits are given by scalar equations
  case SpikeInsensitive:
    break; //FIXME auto with algorithm to detect spikes TBD
  case MeanCentered:
    break; //FIXME the mean of all active curves
  default:
    break;
  }

  *min = minimum;
  *max = maximum;
}


RelationList PlotRenderItem::relationList() const {
  return _relationList;
}


void PlotRenderItem::setRelationList(const RelationList &relationList) {
  _relationList = relationList;
  zoomMaximum();
}


void PlotRenderItem::paint(QPainter *painter) {
  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->drawRect(rect());

#ifdef CURVE_DRAWING_TIME
  QTime time;
  time.start();
#endif

  painter->save();
  painter->setClipRect(rect());
  paintRelations(painter);

  if (_selectionRect.isValid()) {
    painter->setPen(QPen(QBrush(Qt::black), 1.0, Qt::DotLine));
    painter->drawRect(_selectionRect.rect());
  }
  painter->restore();

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->fillPath(checkBox(), Qt::white);
  if (isHovering()) {
    QRectF check = checkBox().boundingRect();
    check.setSize(QSizeF(check.width() / 1.8, check.height() / 1.8));
    check.moveCenter(checkBox().boundingRect().center());
    QPainterPath p;
    p.addEllipse(check);
    painter->fillPath(p, Qt::black);
  }
  if (isTiedZoom()) {
    painter->save();
    painter->setOpacity(0.5);
    painter->fillPath(checkBox(), Qt::black);
    painter->restore();
  }
  painter->drawPath(checkBox());
  painter->restore();

#ifdef CURVE_DRAWING_TIME
  int elapsed = time.elapsed();
  qDebug()<<"curve drawing took" << elapsed << "to render.";
#endif
}


QString PlotRenderItem::leftLabel() const {
  foreach (RelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty())
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::bottomLabel() const {
  foreach (RelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty())
      return relation->xLabel();
  }
  return QString();
}


QString PlotRenderItem::rightLabel() const {
  //FIXME much less than ideal
  QString left = leftLabel();
  foreach (RelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty() && relation->yLabel() != left)
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::topLabel() const {
  //FIXME much less than ideal
  QString bottom = bottomLabel();
  foreach (RelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty() && relation->xLabel() != bottom)
      return relation->xLabel();
  }
  return QString();
}


void PlotRenderItem::createActions() {
  _zoomMaximum = new QAction(tr("Zoom Maximum"), this);
  _zoomMaximum->setShortcut(Qt::Key_M);
  registerShortcut(_zoomMaximum);
  connect(_zoomMaximum, SIGNAL(triggered()), this, SLOT(zoomMaximum()));

  _zoomMaxSpikeInsensitive = new QAction(tr("Zoom Max Spike Insensitive"), this);
  _zoomMaxSpikeInsensitive->setShortcut(Qt::Key_S);
  registerShortcut(_zoomMaxSpikeInsensitive);
  connect(_zoomMaxSpikeInsensitive, SIGNAL(triggered()), this, SLOT(zoomMaxSpikeInsensitive()));
  _zoomMaxSpikeInsensitive->setEnabled(false);

//   _zoomPrevious = new QAction(tr("Zoom Previous"), this);
//   _zoomPrevious->setShortcut(Qt::Key_R);
//   registerShortcut(_zoomPrevious);
//   connect(_zoomPrevious, SIGNAL(triggered()), this, SLOT(_zoomPrevious()));
//   _zoomPrevious->setEnabled(false);

  _zoomYMeanCentered = new QAction(tr("Y-Zoom Mean-centered"), this);
  _zoomYMeanCentered->setShortcut(Qt::Key_A);
  registerShortcut(_zoomYMeanCentered);
  connect(_zoomYMeanCentered, SIGNAL(triggered()), this, SLOT(zoomYMeanCentered()));
  _zoomYMeanCentered->setEnabled(false);

  _zoomXMaximum = new QAction(tr("X-Zoom Maximum"), this);
  _zoomXMaximum->setShortcut(Qt::CTRL+Qt::Key_M);
  registerShortcut(_zoomXMaximum);
  connect(_zoomXMaximum, SIGNAL(triggered()), this, SLOT(zoomXMaximum()));

  _zoomXOut = new QAction(tr("X-Zoom Out"), this);
  _zoomXOut->setShortcut(Qt::SHIFT+Qt::Key_Right);
  registerShortcut(_zoomXOut);
  connect(_zoomXOut, SIGNAL(triggered()), this, SLOT(zoomXOut()));
  _zoomXOut->setEnabled(false);

  _zoomXIn = new QAction(tr("X-Zoom In"), this);
  _zoomXIn->setShortcut(Qt::SHIFT+Qt::Key_Left);
  registerShortcut(_zoomXIn);
  connect(_zoomXIn, SIGNAL(triggered()), this, SLOT(zoomXIn()));
  _zoomXIn->setEnabled(false);

  _zoomNormalizeXtoY = new QAction(tr("Normalize X Axis to Y Axis"), this);
  _zoomNormalizeXtoY->setShortcut(Qt::Key_N);
  registerShortcut(_zoomNormalizeXtoY);
  connect(_zoomNormalizeXtoY, SIGNAL(triggered()), this, SLOT(zoomNormalizeXtoY()));
  _zoomNormalizeXtoY->setEnabled(false);

  _zoomLogX = new QAction(tr("Log X Axis"), this);
  _zoomLogX->setShortcut(Qt::Key_G);
  _zoomLogX->setCheckable(true);
  registerShortcut(_zoomLogX);
  connect(_zoomLogX, SIGNAL(triggered()), this, SLOT(zoomLogX()));

  _zoomYLocalMaximum = new QAction(tr("Y-Zoom Local Maximum"), this);
  _zoomYLocalMaximum->setShortcut(Qt::SHIFT+Qt::Key_L);
  registerShortcut(_zoomYLocalMaximum);
  connect(_zoomYLocalMaximum, SIGNAL(triggered()), this, SLOT(zoomYLocalMaximum()));
  _zoomYLocalMaximum->setEnabled(false);

  _zoomYMaximum = new QAction(tr("Y-Zoom Maximum"), this);
  _zoomYMaximum->setShortcut(Qt::SHIFT+Qt::Key_M);
  registerShortcut(_zoomYMaximum);
  connect(_zoomYMaximum, SIGNAL(triggered()), this, SLOT(zoomYMaximum()));

  _zoomYOut = new QAction(tr("Y-Zoom Out"), this);
  _zoomYOut->setShortcut(Qt::SHIFT+Qt::Key_Up);
  registerShortcut(_zoomYOut);
  connect(_zoomYOut, SIGNAL(triggered()), this, SLOT(zoomYOut()));
  _zoomYOut->setEnabled(false);

  _zoomYIn = new QAction(tr("Y-Zoom In"), this);
  _zoomYIn->setShortcut(Qt::SHIFT+Qt::Key_Down);
  registerShortcut(_zoomYIn);
  connect(_zoomYIn, SIGNAL(triggered()), this, SLOT(zoomYIn()));
  _zoomYIn->setEnabled(false);

  _zoomNormalizeYtoX = new QAction(tr("Normalize Y Axis to X Axis"), this);
  _zoomNormalizeYtoX->setShortcut(Qt::SHIFT+Qt::Key_N);
  registerShortcut(_zoomNormalizeYtoX);
  connect(_zoomNormalizeYtoX, SIGNAL(triggered()), this, SLOT(zoomNormalizeYtoX()));
  _zoomNormalizeYtoX->setEnabled(false);

  _zoomLogY = new QAction(tr("Log Y Axis"), this);
  _zoomLogY->setShortcut(Qt::Key_L);
  _zoomLogY->setCheckable(true);
  registerShortcut(_zoomLogY);
  connect(_zoomLogY, SIGNAL(triggered()), this, SLOT(zoomLogY()));
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
  zoom.addAction(_zoomLogX);

  zoom.addSeparator();

  zoom.addAction(_zoomYLocalMaximum);
  zoom.addAction(_zoomYMaximum);
  zoom.addAction(_zoomYOut);
  zoom.addAction(_zoomYIn);
  zoom.addAction(_zoomNormalizeYtoX);
  zoom.addAction(_zoomLogY);

  menu.addMenu(&zoom);

  menu.exec(event->screenPos());
}


void PlotRenderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  const QPointF p = event->pos();
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
  } else if (modifiers & Qt::ControlModifier) {
    _selectionRect.setTo(QPointF(p.x(), rect().bottom()));
  } else {
    _selectionRect.setTo(p);
  }

  if (_selectionRect.isValid()) {
    update(); //FIXME should optimize instead of redrawing entire curve?
  }
}


void PlotRenderItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  if (checkBox().contains(event->pos())) {
    setTiedZoom(!isTiedZoom());
    update(); //FIXME should optimize instead of redrawing entire curve!
  }

  const QPointF p = event->pos();
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), p.y()));
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
  } else if (modifiers & Qt::ControlModifier) {
    setCursor(Qt::SizeHorCursor);
    _selectionRect.setFrom(QPointF(p.x(), rect().top()));
    _selectionRect.setTo(QPointF(p.x(), rect().bottom()));
  } else {
    _selectionRect.setFrom(p);
  }
}


void PlotRenderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  updateCursor(event->pos());
  const QRectF projection = mapToProjection(_selectionRect.rect());
  _selectionRect.reset();
  setProjectionRect(projection);
}


void PlotRenderItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverMoveEvent(event);

  updateCursor(event->pos());

  const QPointF p = mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(p.x())).arg(QString::number(p.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverEnterEvent(event);

  updateCursor(event->pos());

  const QPointF p = mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(p.x())).arg(QString::number(p.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverLeaveEvent(event);

  updateCursor(event->pos());

  kstApp->mainWindow()->statusBar()->showMessage(QString());
}


QTransform PlotRenderItem::projectionTransform() const {
  QTransform t;

  QRectF v = QRectF(rect().bottomLeft(), viewRect().topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(projectionRect());
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


/*
 * X axis zoom to Auto, Y axis zoom to AutoBorder.
 */
void PlotRenderItem::zoomMaximum() {
  qDebug() << "zoomMaximum" << endl;
  setXAxisZoomMode(Auto);
  setYAxisZoomMode(AutoBorder);
  setProjectionRect(computedProjectionRect());
}


/*
 * X axis zoom to Auto, Y axis zoom to SpikeInsensitive.
 */
void PlotRenderItem::zoomMaxSpikeInsensitive() {
  qDebug() << "FIXME: zoomMaxSpikeInsensitive" << endl;
}


// void PlotRenderItem::zoomPrevious() {
//   qDebug() << "FIXME: zoomPrevious" << endl;
// }


/*
 * X azis zoom to Auto, Y axis zoom to Mean Centered.
 */
void PlotRenderItem::zoomYMeanCentered() {
  qDebug() << "FIXME: zoomYMeanCentered" << endl;
}


/*
 * X axis zoom to auto, Y zoom not changed.
 */
void PlotRenderItem::zoomXMaximum() {
  qDebug() << "zoomXMaximum" << endl;

  setXAxisZoomMode(Auto);
  QRectF compute = computedProjectionRect();
  setProjectionRect(QRectF(compute.x(),
                           _projectionRect.y(),
                           compute.width(),
                           _projectionRect.height()));
}


/*
 * X axis zoom changed to fixed and increased:
 *       new_xmin = xmin - (xmax - xmin)*0.25;
 *       new_xmax = xmax + (xmax – xmin)*0.25;
 */
void PlotRenderItem::zoomXOut() {
  qDebug() << "FIXME: zoomXOut" << endl;
}


/*
 * X axis zoom changed to fixed and decreased:
 *       new_xmin = xmin + (xmax - xmin)*0.1666666;
 *       new_xmax = xmax - (xmax – xmin)*0.1666666;
 */
void PlotRenderItem::zoomXIn() {
  qDebug() << "FIXME: zoomXIn" << endl;
}


/*
 * Normalize X axis to Y axis: Given the current plot aspect ratio, change
 * the X axis range to have the same units per mm as the Y axis range. Particularly
 * useful for images.
 */
void PlotRenderItem::zoomNormalizeXtoY() {
  qDebug() << "FIXME: zoomNormalizeXtoY" << endl;
}


/*
 * Toggle log X axis.
 */
void PlotRenderItem::zoomLogX() {
  qDebug() << "zoomLogX" << endl;
  setXAxisLog(_zoomLogX->isChecked());
  update();
}


/*
 * When zoomed in in X, auto zoom Y, only
 * counting points within the current X range. (eg, curve goes from x=0 to 100, but
 * we are zoomed in to x = 30 to 40. Adjust Y zoom to include all points with x
 * values between 30 and 40.
 */
void PlotRenderItem::zoomYLocalMaximum() {
  qDebug() << "FIXME: zoomYLocalMaximum" << endl;
}


/*
 * Y axis zoom to auto, X zoom not changed.
 */
void PlotRenderItem::zoomYMaximum() {
  qDebug() << "zoomYMaximum" << endl;

  setYAxisZoomMode(Auto);
  QRectF compute = computedProjectionRect();
  setProjectionRect(QRectF(_projectionRect.x(),
                           compute.y(),
                           _projectionRect.width(),
                           compute.height()));
}


/*
 * Y axis zoom increased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin - (ymax - ymin)*0.25;
 *             new_ymax = ymax + (ymax - ymin)*0.25;
 */
void PlotRenderItem::zoomYOut() {
  qDebug() << "FIXME: zoomYOut" << endl;
}


/*
 * Y axis zoom decreased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin + (ymax - ymin)*0.1666666;
 *             new_ymax = ymax - (ymax – ymin)*0.1666666;
 */
void PlotRenderItem::zoomYIn() {
  qDebug() << "FIXME: zoomYIn" << endl;
}


/*
 * Normalize Y axis to X axis: Given the current plot aspect ratio,
 * change the Y axis range to have the same units per mm as the X axis range.
 * Particularly useful for images.
 */
void PlotRenderItem::zoomNormalizeYtoX() {
  qDebug() << "FIXME: zoomNormalizeYtoX" << endl;
}


/*
 * Toggle log Y axis.
 */
void PlotRenderItem::zoomLogY() {
  qDebug() << "zoomLogY" << endl;
  setYAxisLog(_zoomLogY->isChecked());
  update();
}


QPainterPath PlotRenderItem::shape() const {
  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);
  selectPath.addPolygon(rect());
  selectPath.addPath(checkBox());
  return selectPath;
}


QRectF PlotRenderItem::boundingRect() const {
  QPolygonF checkBound = checkBoxBoundingRect();
  return QRectF(checkBound[0], checkBound[2]);
}


QSizeF PlotRenderItem::sizeOfGrip() const {
  return ViewItem::sizeOfGrip() / 1.2;
}


bool PlotRenderItem::maybeReparent() {
  return false; //never reparent a plot renderer
}


QRectF PlotRenderItem::checkBoxBoundingRect() const {
  QRectF bound = selectBoundingRect();
  bound.setTopLeft(bound.topLeft() - QPointF(sizeOfGrip().width(), sizeOfGrip().height()));
  bound.setWidth(bound.width() + sizeOfGrip().width());
  bound.setHeight(bound.height() + sizeOfGrip().height());
  return bound;
}


QPainterPath PlotRenderItem::checkBox() const {
  QRectF bound = checkBoxBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  QPainterPath path;
  path.addEllipse(grip);
  return path;
}


void PlotRenderItem::updateGeometry() {
  setViewRect(plotItem()->plotRegion());
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

void PlotRenderItem::updateCursor(const QPointF &pos) {
  if (checkBox().contains(pos)) {
    setCursor(Qt::ArrowCursor);
  } else {
    updateViewMode();
  }
}

}

// vim: ts=2 sw=2 et
