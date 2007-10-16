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

#include "plotitem.h"

#include "viewitemzorder.h"
#include "plotaxisitem.h"
#include "plotitemmanager.h"
#include "plotrenderitem.h"

#include "layoutboxitem.h"
#include "viewgridlayout.h"
#include "debug.h"

#include "application.h"
#include "mainwindow.h"
#include "tabwidget.h"

#include "datacollection.h"
#include "dataobjectcollection.h"
#include "cartesianrenderitem.h"

#include <QDebug>

static qreal MARGIN_WIDTH = 20.0;
static qreal MARGIN_HEIGHT = 20.0;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent),
  _isTiedZoom(false),
  _isLeftLabelVisible(true),
  _isBottomLabelVisible(true),
  _isRightLabelVisible(true),
  _isTopLabelVisible(true),
  _calculatedMarginWidth(0.0),
  _calculatedMarginHeight(0.0) {

  _axisItem = new PlotAxisItem(this);

  setName("Plot");
  setZValue(PLOT_ZVALUE);
  setBrush(Qt::white);

  PlotItemManager::self()->addPlot(this);
}


PlotItem::~PlotItem() {
  PlotItemManager::self()->removePlot(this);
}


QString PlotItem::plotName() const {
  return name();
}


void PlotItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("plot");
  xml.writeAttribute("tiedzoom", QVariant(_isTiedZoom).toString());
  xml.writeAttribute("leftlabelvisible", QVariant(_isLeftLabelVisible).toString());
  xml.writeAttribute("bottomlabelvisible", QVariant(_isBottomLabelVisible).toString());
  xml.writeAttribute("rightlabelvisible", QVariant(_isRightLabelVisible).toString());
  xml.writeAttribute("toplabelvisible", QVariant(_isTopLabelVisible).toString());
  ViewItem::save(xml);
  _axisItem->saveInPlot(xml);
  foreach (PlotRenderItem *renderer, renderItems()) {
    renderer->saveInPlot(xml);
  }
  xml.writeEndElement();


}


QList<PlotRenderItem*> PlotItem::renderItems() const {
  return _renderers.values();
}


PlotRenderItem *PlotItem::renderItem(PlotRenderItem::RenderType type) {
  if (_renderers.contains(type))
    return _renderers.value(type);

  switch (type) {
  case PlotRenderItem::Cartesian:
    {
      CartesianRenderItem *renderItem = new CartesianRenderItem(this);
      _renderers.insert(type, renderItem);
      return renderItem;
    }
  case PlotRenderItem::Polar:
  case PlotRenderItem::Sinusoidal:
  default:
    return 0;
  }
}


void PlotItem::paint(QPainter *painter) {
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->drawRect(rect());
  painter->restore();

  painter->save();
  painter->translate(QPointF(rect().x(), rect().y()));

  //Calculate and adjust the margins based on the bounds...
  QSizeF margins;
  margins = margins.expandedTo(calculateLeftLabelBound(painter));
  margins = margins.expandedTo(calculateBottomLabelBound(painter));
  margins = margins.expandedTo(calculateRightLabelBound(painter));
  margins = margins.expandedTo(calculateTopLabelBound(painter));

//  qDebug() << "setting margin width" << margins.width() << endl;
  setCalculatedMarginWidth(margins.width());

//  qDebug() << "setting margin height" << margins.height() << endl;
  setCalculatedMarginHeight(margins.height());

//  qDebug() << "=============> leftLabel:" << leftLabel() << endl;
  paintLeftLabel(painter);
//  qDebug() << "=============> bottomLabel:" << bottomLabel() << endl;
  paintBottomLabel(painter);
//  qDebug() << "=============> rightLabel:" << rightLabel() << endl;
  paintRightLabel(painter);
//  qDebug() << "=============> topLabel:" << topLabel() << endl;
  paintTopLabel(painter);

  painter->restore();
}


QRectF PlotItem::plotAxisRect() const {
  //the PlotAxisItem uses this to set its rect
  qreal left = isLeftLabelVisible() ? marginWidth() : 0.0;
  qreal bottom = isBottomLabelVisible() ? marginHeight() : 0.0;
  qreal right = isRightLabelVisible() ? marginWidth() : 0.0;
  qreal top = isTopLabelVisible() ? marginHeight() : 0.0;

  QPointF topLeft(rect().topLeft() + QPointF(left, top));
  QPointF bottomRight(rect().bottomRight() - QPointF(right, bottom));

  return QRectF(topLeft, bottomRight);
}


QRectF PlotItem::plotRect() const {
  //the PlotRenderItems use this to set their rects
  QRectF plot = plotAxisRect();
  plot.setLeft(plot.left() + _axisItem->marginWidth());
  plot.setBottom(plot.bottom() - _axisItem->marginHeight());
  return plot;
}


QRectF PlotItem::projectionRect() const {
  QRectF rect;
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->projectionRect().isEmpty()) {
      if (rect.isValid()) {
        rect = rect.united(renderer->projectionRect());
      } else {
        rect = renderer->projectionRect();
      }
    }
  }

  if (!rect.isValid())
    rect = QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)); //default

  return rect;
}


bool PlotItem::isTiedZoom() const {
  return _isTiedZoom;
}


void PlotItem::setTiedZoom(bool tiedZoom) {
  if (_isTiedZoom == tiedZoom)
    return;

  _isTiedZoom = tiedZoom;

  if (_isTiedZoom)
    PlotItemManager::self()->addTiedZoomPlot(this);
  else
    PlotItemManager::self()->removeTiedZoomPlot(this);

  //FIXME ugh, this is expensive, but need to redraw the renderitems checkboxes...
  update();
}


qreal PlotItem::marginWidth() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotMarginWidth(this);
  } else {
    return calculatedMarginWidth();
  }
}


qreal PlotItem::marginHeight() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotMarginHeight(this);
  } else {
    return calculatedMarginHeight();
  }
}


QString PlotItem::leftLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->leftLabel().isEmpty())
      return renderer->leftLabel();
  }
  return QString();
}


QString PlotItem::bottomLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->bottomLabel().isEmpty())
      return renderer->bottomLabel();
  }
  return QString();
}


QString PlotItem::rightLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->rightLabel().isEmpty())
      return renderer->rightLabel();
  }
  return QString();
}


QString PlotItem::topLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->topLabel().isEmpty())
      return renderer->topLabel();
  }
  return QString();
}


bool PlotItem::isLeftLabelVisible() const {
  return _isLeftLabelVisible;
}


void PlotItem::setLeftLabelVisible(bool visible) {
  if (_isLeftLabelVisible == visible)
    return;

  _isLeftLabelVisible = visible;
  emit labelVisibilityChanged();
}


bool PlotItem::isBottomLabelVisible() const {
  return _isBottomLabelVisible;
}


void PlotItem::setBottomLabelVisible(bool visible) {
  if (_isBottomLabelVisible == visible)
    return;

  _isBottomLabelVisible = visible;
  emit labelVisibilityChanged();
}


bool PlotItem::isRightLabelVisible() const {
  return _isRightLabelVisible;
}


void PlotItem::setRightLabelVisible(bool visible) {
  if (_isRightLabelVisible == visible)
    return;

  _isRightLabelVisible = visible;
  emit labelVisibilityChanged();
}


bool PlotItem::isTopLabelVisible() const {
  return _isTopLabelVisible;
}


void PlotItem::setTopLabelVisible(bool visible) {
  if (_isTopLabelVisible == visible)
    return;

  _isTopLabelVisible = visible;
  emit labelVisibilityChanged();
}


void PlotItem::setLabelsVisible(bool visible) {
  setLeftLabelVisible(visible);
  setRightLabelVisible(visible);
  setBottomLabelVisible(visible);
  setTopLabelVisible(visible);
}


qreal PlotItem::calculatedMarginWidth() const {
  qreal m = qMax(MARGIN_WIDTH, _calculatedMarginWidth);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setCalculatedMarginWidth(qreal marginWidth) {
  qreal before = this->calculatedMarginWidth();
  _calculatedMarginWidth = marginWidth;
  if (before != this->calculatedMarginWidth())
    emit geometryChanged();
}


qreal PlotItem::calculatedMarginHeight() const {
  qreal m = qMax(MARGIN_HEIGHT, _calculatedMarginHeight);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setCalculatedMarginHeight(qreal marginHeight) {
  qreal before = this->calculatedMarginHeight();
  _calculatedMarginHeight = marginHeight;
  if (before != this->calculatedMarginHeight())
    emit geometryChanged();
}


QRectF PlotItem::horizontalLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, width() - 2.0 * calculatedMarginWidth(), calculatedMarginHeight());
  else
    return QRectF(0.0, 0.0, width() - 2.0 * marginWidth(), marginHeight());
}


QRectF PlotItem::verticalLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, calculatedMarginWidth(), height() - 2.0 * calculatedMarginHeight());
  else
    return QRectF(0.0, 0.0, marginWidth(), height() - 2.0 * marginHeight());
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!isLeftLabelVisible())
    return;

  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabelRect = verticalLabelRect(false);
  leftLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));
  painter->drawText(t.mapRect(leftLabelRect), Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
  painter->restore();
}


QSizeF PlotItem::calculateLeftLabelBound(QPainter *painter) {
  if (!isLeftLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
  painter->restore();

  QSizeF margins;
  margins.setWidth(leftLabelBound.height());
  return margins;
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!isBottomLabelVisible())
    return;

  painter->save();
  QRectF bottomLabelRect = horizontalLabelRect(false);
  bottomLabelRect.moveTopLeft(QPointF(marginWidth(), height() - marginHeight()));
  painter->drawText(bottomLabelRect, Qt::TextWordWrap | Qt::AlignCenter, bottomLabel());
  painter->restore();
}


QSizeF PlotItem::calculateBottomLabelBound(QPainter *painter) {
  if (!isBottomLabelVisible())
    return QSizeF();

  QRectF bottomLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                                  Qt::TextWordWrap | Qt::AlignCenter, bottomLabel());

  QSizeF margins;
  margins.setHeight(bottomLabelBound.height());
  return margins;
}


void PlotItem::paintRightLabel(QPainter *painter) {
  if (!isRightLabelVisible())
    return;

  painter->save();
  painter->translate(width() - marginWidth(), 0.0);
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  //same as left but painter is translated
  QRectF rightLabelRect = verticalLabelRect(false);
  rightLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));
  painter->drawText(t.mapRect(rightLabelRect), Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
  painter->restore();
}


QSizeF PlotItem::calculateRightLabelBound(QPainter *painter) {
  if (!isRightLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);
  QRectF rightLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                 Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
  painter->restore();

  QSizeF margins;
  margins.setWidth(rightLabelBound.height());
  return margins;
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (!isTopLabelVisible())
    return;

  painter->save();
  QRectF topLabelRect = horizontalLabelRect(false);
  topLabelRect.moveTopLeft(QPointF(marginWidth(), 0.0));
  painter->drawText(topLabelRect, Qt::TextWordWrap | Qt::AlignCenter, topLabel());
  painter->restore();
}


QSizeF PlotItem::calculateTopLabelBound(QPainter *painter) {
  if (!isTopLabelVisible())
    return QSizeF();

  QRectF topLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                               Qt::TextWordWrap | Qt::AlignCenter, topLabel());

  QSizeF margins;
  margins.setHeight(topLabelBound.height());
  return margins;
}

void CreatePlotCommand::createItem() {
  _item = new PlotItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

void CreatePlotForCurve::createItem() {
  QPointF center = _view->sceneRect().center();
  center -= QPointF(100.0, 100.0);

  _item = new PlotItem(_view);
  _item->setPos(center);
  _item->setViewRect(0.0, 0.0, 200.0, 200.0);
  //_item->setZValue(1);
  _view->scene()->addItem(_item);

  if (_createLayout) {
    _view->createLayout();
  }

  if (_appendToLayout && _view->layoutBoxItem()) {
    _view->layoutBoxItem()->appendItem(_item);
  }

  creationComplete(); //add to undo stack
}


PlotItemFactory::PlotItemFactory()
: GraphicsFactory() {
  registerFactory("plot", this);
}


PlotItemFactory::~PlotItemFactory() {
}


ViewItem* PlotItemFactory::generateGraphics(QXmlStreamReader& xml, View *view, ViewItem *parent) {
  PlotItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "plot") {
        Q_ASSERT(!rc);
        rc = new PlotItem(view);
        if (parent) {
          rc->setParentItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("tiedzoom");
        if (!av.isNull()) {
          rc->setTiedZoom(QVariant(av.toString()).toBool());
        }
        av = attrs.value("leftlabelvisible");
        if (!av.isNull()) {
          rc->setLeftLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("bottomlabelvisible");
        if (!av.isNull()) {
          rc->setBottomLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("rightlabelvisible");
        if (!av.isNull()) {
          rc->setRightLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("toplabelvisible");
        if (!av.isNull()) {
          rc->setTopLabelVisible(QVariant(av.toString()).toBool());
        }
        // TODO add any specialized PlotItem Properties here.
      } else if (xml.name().toString() == "plotaxis") {
        Q_ASSERT(rc);
        validTag = rc->plotAxisItem()->configureFromXml(xml);
      } else if (xml.name().toString() == "cartesianrender") {
        Q_ASSERT(rc);
        PlotRenderItem * renderItem = rc->renderItem(PlotRenderItem::Cartesian);
        if (renderItem) {
          validTag = renderItem->configureFromXml(xml);
        }
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "plot") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating plot object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
