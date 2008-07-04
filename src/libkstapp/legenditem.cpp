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

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LegendItem::LegendItem(PlotItem *parent)
  : ViewItem(parent->parentView()), _plotItem(parent) {
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
  QPen pen = painter->pen();
  pen.setWidth(2);
  painter->setPen(pen);
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
  // TODO create edit Dialog.
//   LegendItemDialog editDialog(this);
//   editDialog.exec();
}


void LegendItem::setAutoLegend(const bool autoLegend) {
  _auto = autoLegend;
}


bool LegendItem::autoLegend() const {
  return _auto;
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
