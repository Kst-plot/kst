/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "boxitem.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>

namespace Kst {

BoxItem::BoxItem(View *parent)
    : ViewItem(parent) {
  setTypeName("Box");
  setBrush(Qt::white);
  applyDialogDefaultsStroke();
  applyDialogDefaultsFill();
}


BoxItem::~BoxItem() {
}


void BoxItem::paint(QPainter *painter) {
  painter->drawRect(rect());
}


void BoxItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("box");
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


void CreateBoxCommand::createItem() {
  _item = new BoxItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


BoxItemFactory::BoxItemFactory()
: GraphicsFactory() {
  registerFactory("box", this);
}


BoxItemFactory::~BoxItemFactory() {
}


ViewItem* BoxItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  BoxItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "box") {
        Q_ASSERT(!rc);
        rc = new BoxItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        // Add any new specialized BoxItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "box") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating box object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
