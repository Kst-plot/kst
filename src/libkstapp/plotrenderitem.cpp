/*********************************************************************************
 *                                                                               *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>              *
 *   copyright : (C) 2000-2010 Barth Netterfield <netterfield@astro.utoronto.ca> *
 *                                                                               *
 *   This program is free software; you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by        *
 *   the Free Software Foundation; either version 2 of the License, or           *
 *   (at your option) any later version.                                         *
 *                                                                               *
 ********************************************************************************/

#include "plotrenderitem.h"

#include "plotitem.h"
#include "plotitemmanager.h"
#include "application.h"
#include "objectstore.h"
#include "updatemanager.h"
#include "sharedaxisboxitem.h"
#include "image.h"
#include "debug.h"
#include "applicationsettings.h"

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
  : ViewItem(parentItem->view()), _referencePointMode(false), _highlightPointActive(false), _invertHighlight(false) {

  setTypeName(tr("Plot Render"));
  setParentViewItem(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAllowedGrips(0);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(updatePlotRect()),
          this, SLOT(updateGeometry()));
  connect(parentItem->view(), SIGNAL(viewModeChanged(View::ViewMode)),
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

  disconnect(this, SIGNAL(geometryChanged()), view(), SLOT(viewChanged()));
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


PlotRenderItem::RenderType PlotRenderItem::renderType() const {
  return _type;
}


void PlotRenderItem::setRenderType(PlotRenderItem::RenderType type) {
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

void PlotRenderItem::setRelationsList(const RelationList &relations) {
  _relationList.clear();

  foreach (const RelationPtr & relation, relations) {
    _relationList.append(relation);
  }
  plotItem()->registerChange();
  UpdateManager::self()->doUpdates(true);
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
  QList<QGraphicsItem*> list = QGraphicsItem::childItems();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
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
    setRenderType((RenderType)av.toString().toInt());
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
        GraphicsFactory::parse(xml, store, view(), this);
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

  if (!view()->isPrinting()) {
    processHoverMoveEvent(_hoverPos);
  }

  paintReferencePoint(painter);
  paintHighlightPoint(painter);

  if (!view()->isPrinting()) {
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
    painter->setRenderHint(QPainter::Antialiasing, ApplicationSettings::self()->antialiasPlots());
    CurvePointSymbol::draw(7, painter, point.x(), point.y(), 7);
    painter->restore();
  }
}


void PlotRenderItem::paintHighlightPoint(QPainter *painter) {
  if (_highlightPointActive && kstApp->mainWindow()->isHighlightPoint() && plotItem()->projectionRect().contains(_highlightPoint)) {
    QPointF point = plotItem()->mapToPlot(_highlightPoint);
    painter->save();
    painter->setPen(QPen(QColor("gray"), 1));
    painter->setBrush(Qt::SolidPattern);
    QColor highlightColor(QColor(0, 0, 0, 127));
    if (_invertHighlight) {
      highlightColor = QColor(255, 255, 255, 127);
    }
    painter->setBrush(highlightColor);

    painter->setRenderHint(QPainter::Antialiasing, ApplicationSettings::self()->antialiasPlots());

    painter->drawEllipse(point, 3, 3);
    painter->restore();
  }
}

QString PlotRenderItem::multiRenderItemLabel(bool isX) const {
  QString units;
  QString quantity;
  LabelInfo label_info;
  LabelInfo first_label_info;

  units.clear();
  quantity.clear();

  bool nameSame = true;
  bool unitsSame = true;
  bool quantitySame = true;

  int count;

  count = relationList().size();
  if (isX) {
    first_label_info = relationList().at(0)->xLabelInfo();
  } else {
    first_label_info = relationList().at(0)->yLabelInfo();
  }
  for (int i=1; i<count; ++i) {
    if (isX) {
      label_info = relationList().at(i)->xLabelInfo();
    } else {
      label_info = relationList().at(i)->yLabelInfo();
    }
    if (label_info.name != first_label_info.name) {
      nameSame = false;
    }
    if (!label_info.units.isEmpty()) {
      if (units.isEmpty()) {
        units = label_info.units;
      } else {
        if (label_info.units != units) {
          unitsSame = false;
        }
      }
    }
    if (!label_info.quantity.isEmpty()) {
      if (quantity.isEmpty()) {
        quantity = label_info.quantity;
      } else {
        if (label_info.quantity != quantity) {
          quantitySame = false;
        }
      }
    }
  }

  if (nameSame && unitsSame && quantitySame) {
    return label_info.singleRenderItemLabel();
  }

  if (!quantitySame) {
    quantity.clear();
  }
  if (!unitsSame) {
    units.clear();
  }

  if (!units.isEmpty() && !quantity.isEmpty()) {
    return QString("%1 \\[%2\\]").arg(quantity).arg(units);
  } else if (!units.isEmpty()) {
    return units;
  } else {
    return quantity;
  }
}


QString PlotRenderItem::leftLabel() const {
  if (relationList().size() == 1) {
    return relationList().at(0)->yLabelInfo().singleRenderItemLabel();
  } else if (relationList().size()>1) {  // multiple curves: quantity [units]
    return multiRenderItemLabel(false);
  } else {
    return QString();
  }
}


QString PlotRenderItem::bottomLabel() const {
  if (relationList().size() == 1) {
    return relationList().at(0)->xLabelInfo().singleRenderItemLabel();
  } else if (relationList().size()>1) {  // multiple curves: quantity [units]
    return multiRenderItemLabel(true);
  } else {
    return QString();
  }
}


QString PlotRenderItem::rightLabel() const {
  // right labels should only be used where there is more than one
  // projection in the plot...
  return QString();
}


QString PlotRenderItem::topLabel() const {
  if (relationList().size() == 1) {
    LabelInfo label_info = relationList().at(0)->titleInfo();
    QString label = label_info.singleRenderItemLabel();
    if (label.isEmpty()) {
      label_info = relationList().at(0)->yLabelInfo();
      if (label_info.singleRenderItemLabel().isEmpty()) {
        label = relationList().at(0)->descriptiveName();
      } else if ((!label_info.name.isEmpty()) && (!label_info.quantity.isEmpty())) {
        LabelInfo xlabel_info = relationList().at(0)->xLabelInfo();
        if ((!xlabel_info.name.isEmpty()) && (!xlabel_info.quantity.isEmpty())) {
          label = i18n("%1 vs %2").arg(label_info.name).arg(xlabel_info.name);
        } else {
          label = label_info.name;
        }
      }
    }
    return label;
  } else {
    return QString();
  }
}


void PlotRenderItem::keyPressEvent(QKeyEvent *event) {
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT) {
    // show cursor as Qt::SizeVerCursor only on mouse events
    // because shift is also used for arrow key controlled zooming
  } else if (modifiers & Qt::CTRL) {
    view()->setCursor(Qt::SizeHorCursor);
  }

  ViewItem::keyPressEvent(event);

  updateSelectionRect();
}


void PlotRenderItem::keyReleaseEvent(QKeyEvent *event) {
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT || zoomOnlyMode() == View::ZoomOnlyY) {
    view()->setCursor(Qt::SizeVerCursor);
  } else if (modifiers & Qt::CTRL || zoomOnlyMode() == View::ZoomOnlyX) {
    view()->setCursor(Qt::SizeHorCursor);
  } else {
    view()->setCursor(Qt::CrossCursor);
    QList<PlotItem*> plots = sharedOrTiedPlots(true, true);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->resetSelectionRect();
    }
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

void PlotRenderItem::wheelEvent(QGraphicsSceneWheelEvent *event) {
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  bool zoom = false;
  if (modifiers & Qt::SHIFT) {
    if (event->delta()>0) {
      plotItem()->zoomYIn();
    } else {
      plotItem()->zoomYOut();
    }
    zoom = true;
  }
  if (modifiers & Qt::CTRL){
    if (event->delta()>0) {
      plotItem()->zoomXIn();
    } else {
      plotItem()->zoomXOut();
    }
    zoom = true;
  }

  if (zoom) {
    return;
  }

  if ((modifiers & Qt::ALT) || zoomOnlyMode() == View::ZoomOnlyY) {
    if (event->delta()>0) {
      plotItem()->zoomYDown();
    } else {
      plotItem()->zoomYUp();
    }
  } else {
    if (event->delta()>0) {
      plotItem()->zoomXLeft();
    } else {
      plotItem()->zoomXRight();
    }
  }
}

void PlotRenderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  const QPointF p = event->pos();

  double y = (p.y() - rect().bottom())/(rect().top()-rect().bottom())*(plotItem()->yMax()-plotItem()->yMin())+plotItem()->yMin();
  y = qMin(y, plotItem()->yMax());
  y = qMax(y, plotItem()->yMin());

  double x = (p.x() - rect().left())/(rect().right()-rect().left())*(plotItem()->xMax()-plotItem()->xMin())+plotItem()->xMin();
  x = qMin(x, plotItem()->xMax());
  x = qMax(x, plotItem()->xMin());


  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT || zoomOnlyMode() == View::ZoomOnlyY) {
    view()->setCursor(Qt::SizeVerCursor);
    QList<PlotItem*> plots = sharedOrTiedPlots(false, true);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->dragYZoomMouseCursor(y);
    }
  } else if (modifiers & Qt::CTRL || zoomOnlyMode() == View::ZoomOnlyX) {
    QList<PlotItem*> plots = sharedOrTiedPlots(true, false);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->dragXZoomMouseCursor(x);
    }
  } else {
    _selectionRect.setTo(p);
  }

  updateSelectionRect();
}


void PlotRenderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  const QPointF point = plotItem()->mapToProjection(event->pos());
  double range = 4.0*(plotItem()->xMax() - plotItem()->xMin())/double(rect().width());
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
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  if (event->button() == Qt::MidButton) {
    plotItem()->zoomPrevious();
    event->ignore();
  }

  const QPointF p = event->pos();
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT || zoomOnlyMode() == View::ZoomOnlyY) {
    view()->setCursor(Qt::SizeVerCursor);
    _selectionRect.setFrom(QPointF(rect().left(), p.y()));
    _selectionRect.setTo(QPointF(rect().right(), p.y()));
  } else if (modifiers & Qt::CTRL || zoomOnlyMode() == View::ZoomOnlyX) {
    view()->setCursor(Qt::SizeHorCursor);
    _selectionRect.setFrom(QPointF(p.x(), rect().top()));
    _selectionRect.setTo(QPointF(p.x(), rect().bottom()));
  } else {
    _selectionRect.setFrom(p);
  }
}


void PlotRenderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  updateCursor(event->pos());
  const QRectF projection = plotItem()->mapToProjection(_selectionRect.rect());

  QList<PlotItem*> plots = sharedOrTiedPlots(true, true);
  foreach (PlotItem *plot, plots) {
    plot->renderItem()->_selectionRect.reset();
  }
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT || zoomOnlyMode() == View::ZoomOnlyY) {
    plotItem()->zoomYRange(projection);
  } else if (modifiers & Qt::CTRL || zoomOnlyMode() == View::ZoomOnlyX) {
    plotItem()->zoomXRange(projection);
  } else {
    plotItem()->zoomFixedExpression(projection);
  }
}

void PlotRenderItem::hoverYZoomMouseCursor(double y) {
  double py;

  py = (y-plotItem()->yMin())/(plotItem()->yMax() - plotItem()->yMin())*(rect().top()-rect().bottom()) + rect().bottom();
  py = qMin(py, rect().bottom());
  py = qMax(py, rect().top());

  _selectionRect.setFrom(QPointF(rect().left(), py));
  _selectionRect.setTo(QPointF(rect().right(), py));

  //qDebug() << "tied: " << plotItem()->isTiedZoom() << PlotItemManager::self()->tiedZoomPlotsForView(view()).size();
  update(); //FIXME should optimize instead of redrawing entire curve!

}

void PlotRenderItem::hoverXZoomMouseCursor(double x) {
  double px;

  px = (x-plotItem()->xMin())/(plotItem()->xMax() - plotItem()->xMin())*(rect().right()-rect().left()) + rect().left();
  px = qMax(px, rect().left());
  px = qMin(px, rect().right());


  _selectionRect.setFrom(QPointF(px, rect().top()));
  _selectionRect.setTo(QPointF(px, rect().bottom()));

  update(); //FIXME should optimize instead of redrawing entire curve!
}

void PlotRenderItem::dragYZoomMouseCursor(double y) {
  double py;

  py = (y-plotItem()->yMin())/(plotItem()->yMax() - plotItem()->yMin())*(rect().top()-rect().bottom()) + rect().bottom();
  py = qMin(py, rect().bottom());
  py = qMax(py, rect().top());

  _selectionRect.setTo(QPointF(rect().right(), py));
  update(); //FIXME should optimize instead of redrawing entire curve!

}

void PlotRenderItem::dragXZoomMouseCursor(double x) {
  double px;

  px = (x-plotItem()->xMin())/(plotItem()->xMax() - plotItem()->xMin())*(rect().right()-rect().left()) + rect().left();
  px = qMax(px, rect().left());
  px = qMin(px, rect().right());

  _selectionRect.setTo(QPointF(px, rect().bottom()));
  update(); //FIXME should optimize instead of redrawing entire curve!
}


QList<PlotItem*> PlotRenderItem::sharedOrTiedPlots(bool sharedX, bool sharedY) {
  QList<PlotItem*> plots;
  QList<PlotItem*> shared_plots;
  QList<PlotItem*> tied_plots;

  if (plotItem()->isInSharedAxisBox()) {
    shared_plots = plotItem()->sharedAxisBox()->getSharedPlots();
    bool keep;
    foreach (PlotItem *plot, shared_plots) {
      keep = (sharedX && plotItem()->sharedAxisBox()->isXAxisShared()) ||
             (sharedY && plotItem()->sharedAxisBox()->isYAxisShared());
      if (keep) {
        plots.append(plot);
      }
    }
  }

  if (plotItem()->isTiedZoom()) {
    tied_plots = PlotItemManager::self()->tiedZoomPlotsForView(view());
    foreach (PlotItem *plot, tied_plots) {
      if (!plots.contains(plot)) {
        plots.append(plot);
      }
    }
  }

  if (plots.size()<1) {
    plots.append(plotItem());
  }

  return plots;
}


//FIXME: store event or pos, and re-call this when window is redrawn
void PlotRenderItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {

  ViewItem::hoverMoveEvent(event);

  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }

  QPointF p = event->pos();

  double y = (p.y() - rect().bottom())/(rect().top()-rect().bottom())*(plotItem()->yMax()-plotItem()->yMin())+plotItem()->yMin();
  double x = (p.x() - rect().left())/(rect().right()-rect().left())*(plotItem()->xMax()-plotItem()->xMin())+plotItem()->xMin();

  _hoverPos = p;
  const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
  if (modifiers & Qt::SHIFT || zoomOnlyMode() == View::ZoomOnlyY) {
    _lastPos = p;
    view()->setCursor(Qt::SizeVerCursor);
    QList<PlotItem*> plots = sharedOrTiedPlots(false,true);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->hoverYZoomMouseCursor(y);
    }
  } else if (modifiers & Qt::CTRL || zoomOnlyMode() == View::ZoomOnlyX) {
    _lastPos = p;
    view()->setCursor(Qt::SizeHorCursor);
    QList<PlotItem*> plots = sharedOrTiedPlots(true,false);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->hoverXZoomMouseCursor(x);
    }
  } else {
    QList<PlotItem*> plots = sharedOrTiedPlots(true,true);
    foreach (PlotItem *plot, plots) {
      plot->renderItem()->resetSelectionRect();
    }
    updateCursor(p);
  }

  processHoverMoveEvent(p);

  if (kstApp->mainWindow()->isHighlightPoint()) {
    update();
  }
}

void PlotRenderItem::processHoverMoveEvent(const QPointF &p) {

  if (p.isNull()) {
    return;
  }

  const QPointF point = plotItem()->mapToProjection(p);
  statusMessagePoint = point;
  if (kstApp->mainWindow()->isHighlightPoint()) {
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

    double distance, minDistance = 1.0E300;
    double x, y;
    QPointF matchedPoint;
    double imageZ;
    double dxPerPix = double(projectionRect().width())/double(rect().width());

    foreach(RelationPtr relation, relationList()) {
      if (Curve* curve = kst_cast<Curve>(relation)) {
        int index = curve->getIndexNearXY(position.x(), dxPerPix, position.y());
        curve->point(index, x, y);
        distance = fabs(position.y() - y);
        if (bFirst || distance < minDistance) {
          matchedPoint = QPointF(x, y);
          statusMessagePoint = matchedPoint;
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
      statusMessagePoint = position;
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
  if (view()->viewMode() != View::Data) {
    event->ignore();
    return;
  }
  
  setFocus(Qt::MouseFocusReason);

  // Qt bug: http://bugreports.qt.nokia.com/browse/QTBUG-8188
  if (!hasFocus()) {
    QEvent activate(QEvent::WindowActivate);
    View* v = view();
    if (v) {
      QApplication::sendEvent(v->scene(), &activate);
      setFocus(Qt::MouseFocusReason);
      if (!hasFocus()) {
        Debug::self()->log("PlotRenderItem::hoverEnterEvent: could not set focus", Debug::Warning);
      }
    }
  }

  updateCursor(event->pos());

  const QPointF p = plotItem()->mapToProjection(event->pos());
  statusMessagePoint = p;
  QString message = QString("(%1, %2)").
                    arg(plotItem()->xAxis()->statusBarString(p.x())).
                    arg(QString::number(p.y()));
  kstApp->mainWindow()->setStatusMessage(message);
}

void PlotRenderItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ViewItem::hoverLeaveEvent(event);

  _hoverPos = QPointF(0,0);

  _highlightPointActive = false;

  if (view()->viewMode() != View::Data) {
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


bool PlotRenderItem::updateViewItemParent() {
  return false; //never reparent a plot renderer
}


void PlotRenderItem::updateGeometry() {
  setViewRect(plotItem()->plotRect());
}


void PlotRenderItem::updateViewMode() {
  switch (view()->viewMode()) {
  case View::Data:
    view()->setCursor(Qt::CrossCursor);
    break;
  case View::Layout:
    view()->setCursor(Qt::ArrowCursor);
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


void PlotRenderItem::createCustomLayout(int columns) {
  plotItem()->createCustomLayout(columns);
}


void PlotRenderItem::remove() {
  plotItem()->remove();
}


void PlotRenderItem::updateCursor(const QPointF &pos) {
  _lastPos = pos;
  if (checkBox().contains(pos)) {
    view()->setCursor(Qt::ArrowCursor);
  } else {
    updateViewMode();
  }
}


QRectF PlotRenderItem::computedProjectionRect() const {
  qreal minX, minY, maxX, maxY;

  //initialize to current projection rect...
  projectionRect().getCoords(&minX, &minY, &maxX, &maxY);

  double minX_d = minX, minY_d = minY, maxX_d = maxX, maxY_d = maxY;

  // FIXME: We should not be using QRectF, because we should
  // always use doubles for data ranges, and QRectF uses qreal
  // which is defined as 'double' on x86 and 'float' on ARM.
  computeXAxisRange(&minX_d, &maxX_d);
  computeYAxisRange(&minY_d, &maxY_d);

  return QRectF(QPointF(minX_d, minY_d),
                QPointF(maxX_d, maxY_d));
}


void PlotRenderItem::computeXAxisRange(double *min, double *max) const {
  double minimum = *min;
  double maximum = *max;

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


void PlotRenderItem::computeYAxisRange(double *min, double *max) const {
  double minimum = *min;
  double maximum = *max;

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


void PlotRenderItem::computeAuto(Qt::Orientation orientation, double *min, double *max) const {
  //The previous values are of no consequence as this algorithm does not depend
  //on the previous values.  So start over so that first active relation initializes.
  double minimum =-0.1;
  double maximum = 0.1;;
  bool unInitialized = true;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      double minPos_ = orientation == Qt::Horizontal ? relation->minPosX() : relation->minPosY();
      double min_ = orientation == Qt::Horizontal ? relation->minX() : relation->minY();
      double max_ = orientation == Qt::Horizontal ? relation->maxX() : relation->maxY();

      if (min_==min_) { // don't use NaN's
        //If the axis is in log mode, the lower extent will be the
        //minimum value larger than zero.
        if (axisLog) {
          minimum = unInitialized ? minPos_ : qMin(minPos_, minimum);
        } else {
          minimum = unInitialized ? min_ : qMin(min_, minimum);
        }
      }

      if (max_==max_) {
        maximum = unInitialized ? max_ : qMax(max_, maximum);
        if (min_==min_) {
          unInitialized = false;
        }
      }
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


void PlotRenderItem::computeBorder(Qt::Orientation orientation, double *min, double *max) const {
  double minimum = *min;
  double maximum = *max;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();
  double logBase = 10.0/*orientation == Qt::Horizontal ? xLogBase() : yLogBase()*/;

  if (axisLog) {
    minimum = log10(minimum)/log10(logBase);
    maximum = maximum > 0.0 ? log10(maximum) : 0.0;
    double d = qAbs(maximum - minimum) * 0.025;
    maximum = pow(logBase, maximum + d);
    minimum = pow(logBase, minimum - d);
  } else {
    double d = qAbs(maximum - minimum) * 0.025;
    maximum += d;
    minimum -= d;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeMeanCentered(Qt::Orientation orientation, double *min, double *max) const {
  double minimum = *min;
  double maximum = *max;

  int count = 0;
  double mid = 0.0;

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      mid += orientation == Qt::Horizontal ? relation->midX() : relation->midY();
      ++count;
  }

  if (count) {
    mid /= double(count);
    double delta = maximum - minimum;
    minimum = mid - delta / 2.0;
    maximum = mid + delta / 2.0;
  }

  *min = minimum;
  *max = maximum;
}


void PlotRenderItem::computeNoSpike(Qt::Orientation orientation, double *min, double *max) const {
  //The previous values are of no consequence as this algorithm does not depend
  //on the previous values.  So start over so that first active relation initializes.
  bool unInitialized = true;

  bool axisLog = orientation == Qt::Horizontal ? plotItem()->xAxis()->axisLog() : plotItem()->yAxis()->axisLog();
  double minimum = axisLog ? 0.0 : -0.1;
  double maximum = 0.2;

  foreach (RelationPtr relation, relationList()) {
      if (relation->ignoreAutoScale())
        continue;

      double minPos_ = orientation == Qt::Horizontal ? relation->minPosX() : relation->minPosY();
      double min_ = orientation == Qt::Horizontal ? relation->ns_minX() : relation->ns_minY();
      double max_ = orientation == Qt::Horizontal ? relation->ns_maxX() : relation->ns_maxY();

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
    if (plotItem()->view()->viewMode() == View::Layout) {
      plotItem()->sharedAxisBox()->triggerContextEvent(event);
      return;
    }
  }
  ViewItem::contextMenuEvent(event);
}


}

// vim: ts=2 sw=2 et
