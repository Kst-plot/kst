/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "legenditem.h"

#include "viewitemzorder.h"
#include <labelparser.h>
#include "labelrenderer.h"

#include "debug.h"
#include "plotitem.h"
#include "legenditemdialog.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LegendItem::LegendItem(PlotItem *parent)
  : ViewItem(parent->parentView()), _plotItem(parent), _auto(true), _verticalDisplay(true) {
  setName("Legend");
  setZValue(LEGEND_ZVALUE);

  setAllowedGripModes(Move /*| Resize*/ /*| Rotate*/ /*| Scale*/);

  QPointF origin = parent->mapToScene(parent->plotRect().topLeft());
  origin += QPointF(parent->width() / 10, parent->height() / 10);

  setPos(origin);
  setViewRect(0.0, 0.0, 50.0, 50.0);
  parentView()->scene()->addItem(this);
}


LegendItem::~LegendItem() {
}


void LegendItem::paint(QPainter *painter) {
  painter->save();
  QRectF box = rect();
  painter->drawRect(rect());
  painter->restore();
}


void LegendItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("legend");
  //TODO Add saving
  ViewItem::save(xml);
  xml.writeEndElement();
}


void LegendItem::edit() {
  LegendItemDialog editDialog(this);
  editDialog.exec();
}


void LegendItem::setAutoContents(const bool autoContents) {
  _auto = autoContents;
}


bool LegendItem::autoContents() const {
  return _auto;
}


void LegendItem::setVerticalDisplay(const bool vertical) {
  _verticalDisplay = vertical;
}


bool LegendItem::verticalDisplay() const {
  return _verticalDisplay;
}


void LegendItem::setTitle(const QString &title) {
  _title = title;
}


QString LegendItem::title() const {
  return _title;
}


QFont LegendItem::font() const {
  return _font;
}


void LegendItem::setFont(const QFont &font) {
  _font = font;
}


qreal LegendItem::fontScale() const {
  return _fontScale;
}


void LegendItem::setFontScale(const qreal scale) {
  _fontScale = scale;
}


LegendItemFactory::LegendItemFactory()
: GraphicsFactory() {
  registerFactory("legend", this);
}


LegendItemFactory::~LegendItemFactory() {
}


ViewItem* LegendItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  //TODO add loading
}


}

// vim: ts=2 sw=2 et
