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
#include "plotitemmanager.h"
#include "application.h"
#include "objectstore.h"
#include "updatemanager.h"
#include "sharedaxisboxitem.h"
#include "application.h"
#include "image.h"

#include <QTime>
#include <QMenu>
#include <QMainWindow>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>

// #define CURVE_DRAWING_TIME

namespace Kst {

PlotRenderItem::PlotRenderItem(PlotItem *parentItem)
  : ViewItem(parentItem->parentView()), _referencePointMode(false), _highlightPointActive(false), _invertHighlight(false) {

  setTypeName(tr("Plot Render"));
  setParentViewItem(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAllowedGrips(0);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(updatePlotRect()),
          this, SLOT(updateGeometry()));
  connect(parentItem->parentView(), SIGNAL(viewModeChanged(View::ViewMode)),
          this, SLOT(updateViewMode()));

  updateGeometry(); //the initial rect
  updateViewMode(); //the initial view

  _referenceMode = new QAction(tr("Reference Mode"), this);
  _referenceMode->setShortcut(Qt::Key_C);
  registerShortcut(_referenceMode);
  connect(_referenceMode, SIGNAL(triggered()), this, SLOT(referenceMode()));

  _referenceModeDisabled = new QAction(tr("Reference Mode"), this);
  _referenceModeDisabled->setShortcut(Qt::SHIFT + Qt::Key_C);
  registerShortcut(_referenceModeDisabled);
  connect(_referenceModeDisabled, SIGNAL(triggered()), this, SLOT(referenceModeDisabled()));

  disconnect(this, SIGNAL(geometryChanged()), parentView(), SLOT(viewChanged()));
}


PlotRenderItem::~PlotRenderItem() {
}


PlotItem *PlotRenderItem::plotItem() const {
  return static_cast<PlotItem*>(parentItem());
}


void PlotRenderItem::referenceMode() {
  _referencePointMode = true;
  if (_highlightPointActive) {
    _referencePoint = _highlightPoint;
  } else {
    _referencePoint = plotItem()->mapToProjection(_lastPos);
  }
  update();
}


void PlotRenderItem::referenceModeDisabled() {
  _referencePointMode = false;
}


void PlotRenderItem::setReferencePoint(const QPointF& point) {
  _referencePointMode = true;
  _referencePoint = point;
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
  if (relation) {
    _relationList.append(relation);
    if (_relationList.count() == 1) {
      plotItem()->zoomMaximum();
      plotItem()->xAxis()->setAxisReversed(relation->invertXHint());
      plotItem()->yAxis()->setAxisReversed(relation->invertYHint());
    }
  }
}


void PlotRenderItem::removeRelation(RelationPtr relation) {
  if (relation) {
    _relationList.removeAll(relation);
    plotItem()->registerChange();
    UpdateManager::self()->doUpdates(true);
  }
}


void PlotRenderItem::clearRelations() {
  _relationList.clear();
  plotItem()->zoomMaximum();
}


void PlotRenderItem::save(QXmlStreamWriter &xml) {
  Q_UNUSED(xml);
}


void PlotRenderItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeAttribute("name", typeName());
  xml.writeAttribute("type", QVariant(_type).toString());
  if (_referencePointMode) {
    xml.writeStartElement("referencepoint");
    xml.writeAttribute("x", QVariant(_referencePoint.x()).toString());
    xml.writeAttribute("y", QVariant(_referencePoint.y()).toString());
    xml.writeEndElement();
  }
  foreach (RelationPtr relation, relationList()) {
    xml.writeStartElement("relation");
    xml.writeAttribute("tag", relation->Name());
    xml.writeEndElement();
  }
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    viewItem->save(xml);
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
      QString tagName = attrs.value("tag").toString();
      RelationPtr relation = kst_cast<Relation>(store->retrieveObject(tagName));
      if (relation) {
        addRelation(relation);
      }
    } else if (xml.isStartElement() && xml.name().toString() == "referencepoint") {
      expectedEnd = xml.name().toString();
      double x = 0, y = 0;
      attrs = xml.attributes();
      av = attrs.value("x");
      if (!av.isNull()) {
        x = av.toString().toDouble();
      }
      av = attrs.value("y");
      if (!av.isNull()) {
        y = av.toString().toDouble();
     }
     setReferencePoint(QPointF(x, y));
    } else if (xml.isStartElement()) {
      if (!parse(xml, validTag) && validTag) {
        GraphicsFactory::parse(xml, store, parentView(), this);
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
  if (!rect().isValid()) {
    return;
  }
  if (plotItem()->maskedByMaximization()) {
    return;
  }

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

  painter->restore();

  if (!parentView()->isPrinting()) {
    processHoverMoveEvent(_hoverPos);
  }

  paintReferencePoint(painter);
  paintHighlightPoint(painter);

  if (!parentView()->isPrinting()) {
    if (_selectionRect.isValid()) {
      painter->setPen(QPen(QBrush(Qt::black), 1.0, Qt::DotLine));
      painter->drawRect(_selectionRect.rect());
    }
  }

#ifdef CURVE_DRAWING_TIME
  int elapsed = time.elapsed();
  qDebug()<<"curve drawing took" << elapsed << "to render.";
#endif
}


void PlotRenderItem::paintReferencePoint(QPainter *painter) {
  if (_referencePointMode && plotItem()->projectionRect().contains(_referencePoint)) {
    QPointF point = plotItem()->mapToPlot(_referencePoint);
    painter->save();
    painter->setPen(QPen(QColor("gray"), 1));
    CurvePointSymbol::draw(7, painter, point.x(), point.y(), 1);
    painter->restore();
  }
}


void PlotRenderItem::paintHighlightPoint(QPainter *painter) {
  if (_highlightPointActive && kstApp->mainWindow()->isDataMode() && plotItem()->projectionRect().contains(_highlightPoint)) {
    QPointF point = plotItem()->mapToPlot(_highlightPoint);
    painter->save();
    painter->setPen(QPen(QColor("gray"), 1));
    painter->setBrush(Qt::SolidPattern);
    QColor highlightColor(QColor(0, 0, 0, 127));
    if (_invertHighlight) {
      highlightColor = QColor(255, 255, 255, 127);
    }
    painter->setBrush(highlightColor);
    painter->drawEllipse(point.x()-3, point.y()-3, 7, 7);
    painter->restore();
  }
}


QString PlotRenderItem::leftLabel() const {
  QStringList labels;

  // chose the first vector with a label.
  // for best results here, vectors should define
  // fieldScalars "quantity" and "units".
  foreach (RelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty()) {
      return relation->yLabel();
    }
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
  // right labels should only be used where there is more than one
  // projection in the plot...
  return QString();
}


QString PlotRenderItem::topLabel() const {
  QString label;
  for (int i = 0, count = relationList().count(); i<count; i++) {
    if (i>0) {
      if (i==count-1) {
        //label += i18n(" and ", "last separater in a list");
        label += i18n(" and ");
      } else {
        //label += i18n(", ", "separater in a list");
        label += i18n(", ");
      }
    }
    label += relationList().at(i)->topLabel();
  }
  return label;
}


void PlotRenderItem::keyPressEvent(QKeyEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    parentView()->setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), _lastPos.y()));
    _selectionRect.setTo(QPointF(rect().right(), _lastPos.y()));
  } else if (modifiers & Qt::ControlModifier) {
    parentView()->setCursor(Qt::SizeHorCursor);
    _selectionRect.setFrom(QPointF(_lastPos.x(), rect().top()));
    _selectionRect.setTo(QPointF(_lastPos.x(), rect().bottom()));
  }

  ViewItem::keyPressEvent(event);

  updateSelectionRect();
}


void PlotRenderItem::keyReleaseEvent(QKeyEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    parentView()->setCursor(Qt::SizeVerCursor);
  } else if (modifiers & Qt::ControlModifier) {
    parentView()->setCursor(Qt::SizeHorCursor);
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


void PlotRenderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  const QPointF point = plotItem()->mapToProjection(event->pos());
  qreal range = qMax(plotItem()->xMax() - plotItem()->xMin(), plotItem()->yMax() - plotItem()->yMin());
  double distance = 1000;
  bool first = true;
  RelationPtr closestRelation = 0;
  foreach (RelationPtr relation, _relationList) {
    double relationsDistance = relation->distanceToPoint(point.x(), range, point.y());
    if (first) {
      distance = relationsDistance;
      closestRelation = relation;
      first = false;
    } else {
      if (distance > relationsDistance) {
        distance = relationsDistance;
        closestRelation = relation;
      }
    }
  }
  if (closestRelation) {
    closestRelation->showEditDialog();
  }
}


void PlotRenderItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  if (event->button() == Qt::MidButton) {
    plotItem()->zoomPrevious();
    event->ignore();
  }

  const QPointF p = event->pos();
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    parentView()->setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), p.y()));
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
  } else if (modifiers & Qt::ControlModifier) {
    parentView()->setCursor(Qt::SizeHorCursor);
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

  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    plotItem()->zoomYRange(projection);
  } else if (modifiers & Qt::ControlModifier) {
    plotItem()->zoomXRange(projection);
  } else {
    plotItem()->zoomFixedExpression(projection);
  }
}

//FIXME: store event or pos, and re-call this when window is redrawn
void PlotRenderItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {

  ViewItem::hoverMoveEvent(event);

  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  QPointF p = event->pos();
  _hoverPos = p;
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::ShiftModifier) {
    _lastPos = p;
    parentView()->setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), p.y()));
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
    update(); //FIXME should optimize instead of redrawing entire curve!
  } else if (modifiers & Qt::ControlModifier) {
    _lastPos = p;
    parentView()->setCursor(Qt::SizeHorCursor);
    _selectionRect.setFrom(QPointF(p.x(), rect().top()));
    _selectionRect.setTo(QPointF(p.x(), rect().bottom()));
    update(); //FIXME should optimize instead of redrawing entire curve!
  } else {
    resetSelectionRect();
    updateCursor(p);
  }

  processHoverMoveEvent(p);

  if (kstApp->mainWindow()->isDataMode()) update();
}

void PlotRenderItem::processHoverMoveEvent(const QPointF &p) {

  if (p.isNull()) {
    return;
  }

  const QPointF point = plotItem()->mapToProjection(p);
  if (kstApp->mainWindow()->isDataMode()) {
    highlightNearestDataPoint(point);
  } else {
    _highlightPointActive = false;
    QString message = QString("(%1, %2)").
                      arg(plotItem()->xAxis()->statusBarString(point.x())).
                      arg(QString::number(point.y()));
    if (_referencePointMode) {
      message += QString(" [Offset: %1, %2]").arg(QString::number(point.x() - _referencePoint.x(), 'G')).arg(QString::number(point.y() - _referencePoint.y()));
    }
    kstApp->mainWindow()->setStatusMessage(message);
  }
}


void PlotRenderItem::highlightNearestDataPoint(const QPointF& position) {
  QString curveMsg;
  QString imageMsg;

  _highlightPointActive = false;
  _invertHighlight = false;

  if (!relationList().isEmpty()) {
    QString curveName, imageName;

    bool bFirst = true;
    bool bFoundImage = false;

    qreal distance, minDistance = 1.0E300;
    qreal x, y;
    QPointF matchedPoint;
    qreal imageZ;
    qreal dxPerPix = double(projectionRect().width())/double(rect().width());

    foreach(RelationPtr relation, relationList()) {
      if (Curve* curve = kst_cast<Curve>(relation)) {
        int index = curve->getIndexNearXY(position.x(), dxPerPix, position.y());
        curve->point(index, x, y);
        distance = fabs(position.y() - y);
        if (bFirst || distance < minDistance) {
          matchedPoint = QPointF(x, y);
          bFirst = false;
          minDistance = distance;
          curveName = curve->CleanedName();
          if (curve->color() == Qt::black) {
            _invertHighlight = true;
          }
        }
      } else if (Image* image = kst_cast<Image>(relation)) {
        if (!bFoundImage && image->getNearestZ(position.x(), position.y(), imageZ)) {
          bFoundImage = true;
          imageName = image->CleanedName();
        }
      }
    }
    if (!curveName.isEmpty()) {
      QString message = curveName + QString(" (%1, %2)").
                        arg(plotItem()->xAxis()->statusBarString(matchedPoint.x())).
                        arg(QString::number(matchedPoint.y()));
      if (_referencePointMode) {
        message += QString(" [Offset: %1, %2]").
                   arg(QString::number(matchedPoint.x() - _referencePoint.x(), 'G')).
                   arg(QString::number(matchedPoint.y() - _referencePoint.y()));
      }
      kstApp->mainWindow()->setStatusMessage(message);
      _highlightPointActive = true;
      _highlightPoint = QPointF(matchedPoint.x(), matchedPoint.y());
    } else if (!imageName.isEmpty()) {
      QString message = imageName + QString(" (%1, %2, %3)").
                        arg(plotItem()->xAxis()->statusBarString(position.x())).
                        arg(QString::number(position.y())).
                        arg(QString::number(imageZ, 'G'));
      kstApp->mainWindow()->setStatusMessage(message);
    }
  }
}


void PlotRenderItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverEnterEvent(event);
  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }
  
  setFocus(Qt::MouseFocusReason);

  // Qt bug (Windows only?): http://bugreports.qt.nokia.com/browse/QTBUG-8188
  // Kst: http://bugs.kde.org/show_bug.cgi?id=247634
  //bool foc = hasFocus(); // foc is false in the second tab

  updateCursor(event->pos());

  const QPointF p = plotItem()->mapToProjection(event->pos());
  QString message = QString("(%1, %2)").
                    arg(plotItem()->xAxis()->statusBarString(p.x())).
                    arg(QString::number(p.y()));
  kstApp->mainWindow()->setStatusMessage(message);
}

void PlotRenderItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverLeaveEvent(event);

  _hoverPos = QPointF(0,0);

  _highlightPointActive = false;

  if (parentView()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  clearFocus();
  resetSelectionRect();

  updateCursor(event->pos());

  kstApp->mainWindow()->setStatusMessage(QString());
}


QPainterPath PlotRenderItem::shape() const {
  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);
  selectPath.addPolygon(rect());
  selectPath.addPath(checkBox());
  return selectPath;
}


bool PlotRenderItem::maybeReparent() {
  return false; //never reparent a plot renderer
}


void PlotRenderItem::updateGeometry() {
  setViewRect(plotItem()->plotRect());
}


void PlotRenderItem::updateViewMode() {
  switch (parentView()->viewMode()) {
  case View::Data:
    parentView()->setCursor(Qt::CrossCursor);
    break;
  case View::Layout:
    parentView()->setCursor(Qt::ArrowCursor);
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


void PlotRenderItem::createAutoLayout() {
  plotItem()->createAutoLayout();
}


void PlotRenderItem::createCustomLayout() {
  plotItem()->createCustomLayout();
}


void PlotRenderItem::remove() {
  plotItem()->remove();
}


void PlotRenderItem::updateCursor(const QPointF &pos) {
  _lastPos = pos;
  if (checkBox().contains(pos)) {
    parentView()->setCursor(Qt::ArrowCursor);
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
  case PlotAxis::SpikeInsensitive: //auto with algorithm to detect spikes TBD
    computeNoSpike(Qt::Horizontal, &minimum, &maximum);
    break;
  case PlotAxis::MeanCentered: //the mean of all active curves
    computeMeanCentered(Qt::Horizontal, &minimum, &maximum);
    break;
  case PlotAxis::FixedExpression: // limits are set by user interaction
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
  case PlotAxis::SpikeInsensitive: //auto with algorithm to detect spikes TBD
    computeNoSpike(Qt::Vertical, &minimum, &maximum);
    break;
  case PlotAxis::MeanCentered: //the mean of all active curves
    computeMeanCentered(Qt::Vertical, &minimum, &maximum);
    break;
  case PlotAxis::FixedExpression: // limits are set by user interaction
  default:
    break;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeAuto(Qt::Orientation orientation, qreal *min, qreal *max) const {
  //The previous values are of no consequence as this algorithm does not depend
  //on the previous values.  So start over so that first active relation initializes.
  qreal minimum=-0.1;
  qreal maximum= 0.1;;
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

  if (axisLog) {
    if (unInitialized){ 
      maximum = 100.0;
      minimum = 0.01;
    }
    if (minimum < 0) {
      minimum = 0;
    }
    if (minimum >= maximum) {
      if (minimum <=0) maximum = 1;
      else maximum = minimum * 1.1;
      minimum = minimum * 0.9;
    }
  } else {
    if (maximum <= minimum) {
      minimum = maximum -0.1;
      maximum += 0.1;
    }
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


void PlotRenderItem::computeNoSpike(Qt::Orientation orientation, qreal *min, qreal *max) const {
  //The previous values are of no consequence as this algorithm does not depend
  //on the previous values.  So start over so that first active relation initializes.
  bool unInitialized = true;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();
  qreal minimum = axisLog ? 0.0 : -0.1;
  qreal maximum = 0.2;

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      qreal minPos_ = orientation == Qt::Horizontal ? relation->minPosX() : relation->minPosY();
      qreal min_ = orientation == Qt::Horizontal ? relation->ns_minX() : relation->ns_minY();
      qreal max_ = orientation == Qt::Horizontal ? relation->ns_maxX() : relation->ns_maxY();

      //If the axis is in log mode, the lower extent will be the
      //minimum value larger than zero.
      if (axisLog)
        minimum = unInitialized ? minPos_ : qMin(minPos_, minimum);
      else
        minimum = unInitialized ? min_ : qMin(min_, minimum);

      maximum = unInitialized ? max_ : qMax(max_, maximum);

      unInitialized = false;
  }

  if (maximum <= minimum) {
    minimum = axisLog ? 0.0 : -0.1;
    maximum = 0.2;
  }

  if (axisLog && minimum < 0.0) {
    minimum = pow(10, -350.0);
  }

  *min = minimum;
  *max = maximum;
}


bool PlotRenderItem::tryShortcut(const QString &keySequence) {
  if (ViewItem::tryShortcut(keySequence)) {
    return true;
  } else {
    return plotItem()->tryShortcut(keySequence);
  }
}


void PlotRenderItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  if (plotItem() && plotItem()->parentItem() && plotItem()->isInSharedAxisBox()) {
    if (plotItem()->parentView()->viewMode() == View::Layout) {
      plotItem()->sharedAxisBox()->triggerContextEvent(event);
      return;
    }
  }
  ViewItem::contextMenuEvent(event);
}


}

// vim: ts=2 sw=2 et
