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

#include "plotitem.h"
#include "viewitemzorder.h"
#include "plotitemmanager.h"
#include "application.h"
#include "objectstore.h"

#include <QTime>
#include <QMenu>
#include <QStatusBar>
#include <QMainWindow>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>

// #define CURVE_DRAWING_TIME

namespace Kst {

PlotRenderItem::PlotRenderItem(PlotItem *parentItem)
  : ViewItem(parentItem->parentView()) {

  setName(tr("Plot Render"));
  setZValue(PLOTRENDER_ZVALUE);
  setParent(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAllowedGrips(0);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(marginsChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(updatePlotRect()),
          this, SLOT(updateGeometry()));
  connect(parentItem->parentView(), SIGNAL(viewModeChanged(View::ViewMode)),
          this, SLOT(updateViewMode()));

  updateGeometry(); //the initial rect
  updateViewMode(); //the initial view
}


PlotRenderItem::~PlotRenderItem() {
}


PlotItem *PlotRenderItem::plotItem() const {
  return qobject_cast<PlotItem*>(qgraphicsitem_cast<ViewItem*>(parentItem()));
}


PlotRenderItem::RenderType PlotRenderItem::type() {
  return _type;
}


void PlotRenderItem::setType(PlotRenderItem::RenderType type) {
  _type = type;
}


QRectF PlotRenderItem::plotRect() const {
  QRectF plotRect = rect();
  plotRect = plotRect.normalized();
  plotRect.moveTopLeft(QPointF(0.0, 0.0));
  return plotRect;
}


QRectF PlotRenderItem::projectionRect() const {
    return plotItem()->projectionRect();
}


RelationList PlotRenderItem::relationList() const {
  return _relationList;
}


void PlotRenderItem::addRelation(RelationPtr relation) {
  _relationList.append(relation);
  plotItem()->zoomMaximum();
}


void PlotRenderItem::removeRelation(RelationPtr relation) {
  _relationList.removeAll(relation);
  plotItem()->zoomMaximum();
}


void PlotRenderItem::clearRelations() {
  _relationList.clear();
  plotItem()->zoomMaximum();
}


void PlotRenderItem::save(QXmlStreamWriter &xml) {
  Q_UNUSED(xml);
}


void PlotRenderItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeAttribute("name", name());
  xml.writeAttribute("type", QVariant(_type).toString());
  foreach (RelationPtr relation, relationList()) {
    xml.writeStartElement("relation");
    xml.writeAttribute("tag", relation->tag().tagString());
    xml.writeEndElement();
  }
}


bool PlotRenderItem::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av;
  av = attrs.value("type");
  if (!av.isNull()) {
    setType((RenderType)av.toString().toInt());
  }

  QString expectedEnd;
  while (!(xml.isEndElement() && (xml.name().toString() == primaryTag))) {
   if (xml.isStartElement() && xml.name().toString() == "relation") {
      expectedEnd = xml.name().toString();
      attrs = xml.attributes();
      ObjectTag tag = ObjectTag::fromString(attrs.value("tag").toString());
      qDebug(tag.tagString());
      RelationPtr relation = kst_cast<Relation>(store->retrieveObject(tag));
      if (relation) {
        addRelation(relation);
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() != expectedEnd) {
        validTag = false;
        break;
      }
    }
    xml.readNext();
  }

  return validTag;
}


void PlotRenderItem::addToMenuForContextEvent(QMenu &menu) {
  plotItem()->addToMenuForContextEvent(menu);
}


void PlotRenderItem::paint(QPainter *painter) {
  painter->setRenderHint(QPainter::Antialiasing, false);

#ifdef CURVE_DRAWING_TIME
  QTime time;
  time.start();
#endif

  painter->save();

  if (plotItem()->xAxis()->axisReversed()) {
    painter->scale(-1, 1);
    painter->translate(-1.0 * rect().right() - rect().left(), 0);
  }
  if (plotItem()->yAxis()->axisReversed()) {
    painter->scale(1, -1);
    painter->translate(0, -1.0 * rect().bottom() - rect().top());
  }
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
    QRectF check = checkBox().controlPointRect();
    check.setSize(QSizeF(check.width() / 1.8, check.height() / 1.8));
    check.moveCenter(checkBox().controlPointRect().center());
    QPainterPath p;
    p.addEllipse(check);
    painter->fillPath(p, Qt::black);
  }
  if (plotItem()->isTiedZoom()) {
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


void PlotRenderItem::keyPressEvent(QKeyEvent *event) {
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    setCursor(Qt::SizeVerCursor);
  _selectionRect.setFrom(QPointF(rect().left(), _lastPos.y()));
  _selectionRect.setTo(QPointF(rect().right(), _lastPos.y()));
  } else if (modifiers & Qt::ControlModifier) {
    setCursor(Qt::SizeHorCursor);
  _selectionRect.setFrom(QPointF(_lastPos.x(), rect().top()));
  _selectionRect.setTo(QPointF(_lastPos.x(), rect().bottom()));
  }
  ViewItem::keyPressEvent(event);

  updateSelectionRect();
}


void PlotRenderItem::keyReleaseEvent(QKeyEvent *event) {
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
   setCursor(Qt::SizeVerCursor);
  } else if (modifiers & Qt::ControlModifier) {
    setCursor(Qt::SizeHorCursor);
  } else {
    resetSelectionRect();
  }
  ViewItem::keyReleaseEvent(event);
}


void PlotRenderItem::resetSelectionRect() {
  if (_selectionRect.isValid()) {
    _selectionRect.reset();
    updateCursor(_lastPos);
    update();
  }
}


void PlotRenderItem::updateSelectionRect() {
  if (_selectionRect.isValid()) {
    update(); //FIXME should optimize instead of redrawing entire curve?
  }
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

  updateSelectionRect();
}


void PlotRenderItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  if (checkBox().contains(event->pos())) {
    plotItem()->setTiedZoom(!plotItem()->isTiedZoom());
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
  const QRectF projection = plotItem()->mapToProjection(_selectionRect.rect());
  _selectionRect.reset();

  plotItem()->zoomFixedExpression(projection);
}


void PlotRenderItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverMoveEvent(event);

  const QPointF p = event->pos();
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    _lastPos = p;
    setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), p.y()));
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
    update(); //FIXME should optimize instead of redrawing entire curve!
  } else if (modifiers & Qt::ControlModifier) {
    _lastPos = p;
    setCursor(Qt::SizeHorCursor);
    _selectionRect.setFrom(QPointF(p.x(), rect().top()));
    _selectionRect.setTo(QPointF(p.x(), rect().bottom()));
    update(); //FIXME should optimize instead of redrawing entire curve!
  } else {
    resetSelectionRect();
  }
  const QPointF point = plotItem()->mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(point.x(), 'G')).arg(QString::number(point.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverEnterEvent(event);

  updateCursor(event->pos());

  const QPointF p = plotItem()->mapToProjection(event->pos());
  QString message = QString("(%1, %2)").arg(QString::number(p.x())).arg(QString::number(p.y()));
  kstApp->mainWindow()->statusBar()->showMessage(message);
}


void PlotRenderItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverLeaveEvent(event);

  updateCursor(event->pos());

  kstApp->mainWindow()->statusBar()->showMessage(QString());
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
  setViewRect(plotItem()->plotRect());
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


void PlotRenderItem::edit() {
  plotItem()->edit();
}


void PlotRenderItem::raise() {
  plotItem()->raise();
}


void PlotRenderItem::lower() {
  plotItem()->lower();
}


void PlotRenderItem::createLayout() {
  plotItem()->createLayout();
}


void PlotRenderItem::breakLayout() {
  plotItem()->breakLayout();
}


void PlotRenderItem::remove() {
  plotItem()->remove();
}


void PlotRenderItem::updateCursor(const QPointF &pos) {
  _lastPos = pos;
  if (checkBox().contains(pos)) {
    setCursor(Qt::ArrowCursor);
  } else {
    updateViewMode();
  }
}


QRectF PlotRenderItem::computedProjectionRect() const {
  qreal minX, minY, maxX, maxY;

  //initialize to current projection rect...
  projectionRect().getCoords(&minX, &minY, &maxX, &maxY);

  computeXAxisRange(&minX, &maxX);
  computeYAxisRange(&minY, &maxY);

  return QRectF(QPointF(minX, minY),
                QPointF(maxX, maxY));
}


void PlotRenderItem::computeXAxisRange(qreal *min, qreal *max) const {
  qreal minimum = *min;
  qreal maximum = *max;

  switch (plotItem()->xAxis()->axisZoomMode()) {
  case PlotAxis::Auto:
    computeAuto(Qt::Horizontal, &minimum, &maximum);
    break;
  case PlotAxis::AutoBorder: //auto mode, plus a 2.5% border on top and bottom.
    computeAuto(Qt::Horizontal, &minimum, &maximum);
    computeBorder(Qt::Horizontal, &minimum, &maximum);
    break;
  case PlotAxis::FixedExpression: //limits are given by scalar equations, or mouse
    qDebug() << "FIXME! Need a GUI for entering scalar equations..." << endl;
    break;
  case PlotAxis::SpikeInsensitive: //auto with algorithm to detect spikes TBD
    qDebug() << "FIXME! Need a spike insensitive algorithm..." << endl;
    break;
  case PlotAxis::MeanCentered: //the mean of all active curves
    computeMeanCentered(Qt::Horizontal, &minimum, &maximum);
    break;
  default:
    break;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeYAxisRange(qreal *min, qreal *max) const {
  qreal minimum = *min;
  qreal maximum = *max;

  switch (plotItem()->yAxis()->axisZoomMode()) {
  case PlotAxis::Auto:
    computeAuto(Qt::Vertical, &minimum, &maximum);
    break;
  case PlotAxis::AutoBorder: //auto mode, plus a 2.5% border on top and bottom.
    computeAuto(Qt::Vertical, &minimum, &maximum);
    computeBorder(Qt::Vertical, &minimum, &maximum);
    break;
  case PlotAxis::FixedExpression: //limits are given by scalar equations, or mouse
    qDebug() << "FIXME! Need a GUI for entering scalar equations..." << endl;
    break;
  case PlotAxis::SpikeInsensitive: //auto with algorithm to detect spikes TBD
    qDebug() << "FIXME! Need a spike insensitive algorithm..." << endl;
    break;
  case PlotAxis::MeanCentered: //the mean of all active curves
    computeMeanCentered(Qt::Vertical, &minimum, &maximum);
    break;
  default:
    break;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeAuto(Qt::Orientation orientation, qreal *min, qreal *max) const {
  //The previous values are of no consequence as this algorithm does not depend
  //on the previous values.  So start over so that first active relation initializes.
  qreal minimum;
  qreal maximum;
  bool unInitialized = true;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      qreal minPos_ = orientation == Qt::Horizontal ? relation->minPosX() : relation->minPosY();
      qreal min_ = orientation == Qt::Horizontal ? relation->minX() : relation->minY();
      qreal max_ = orientation == Qt::Horizontal ? relation->maxX() : relation->maxY();

      //If the axis is in log mode, the lower extent will be the
      //minimum value larger than zero.
      if (axisLog)
        minimum = unInitialized ? minPos_ : qMin(minPos_, minimum);
      else
        minimum = unInitialized ? min_ : qMin(min_, minimum);

      maximum = unInitialized ? max_ : qMax(max_, maximum);

      unInitialized = false;
  }

  if (unInitialized || maximum <= minimum) {
    minimum = axisLog ? 0.0 : -0.1;
    minimum = 0.1;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeBorder(Qt::Orientation orientation, qreal *min, qreal *max) const {
  qreal minimum = *min;
  qreal maximum = *max;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();
  qreal logBase = 10.0/*orientation == Qt::Horizontal ? xLogBase() : yLogBase()*/;

  if (axisLog) {
    minimum = log10(minimum)/log10(logBase);
    maximum = maximum > 0.0 ? log10(maximum) : 0.0;
    qreal d = qAbs(maximum - minimum) * 0.025;
    maximum = pow(logBase, maximum + d);
    minimum = pow(logBase, minimum - d);
  } else {
    qreal d = qAbs(maximum - minimum) * 0.025;
    maximum += d;
    minimum -= d;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeMeanCentered(Qt::Orientation orientation, qreal *min, qreal *max) const {
  qreal minimum = *min;
  qreal maximum = *max;

  int count = 0;
  qreal mid = 0.0;

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      mid += orientation == Qt::Horizontal ? relation->midX() : relation->midY();
      ++count;
  }

  if (count) {
    mid /= qreal(count);
    qreal delta = maximum - minimum;
    minimum = mid - delta / 2.0;
    maximum = mid + delta / 2.0;
  }

  *min = minimum;
  *max = maximum;
}

}

// vim: ts=2 sw=2 et
