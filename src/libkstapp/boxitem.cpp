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

#include "boxitem.h"

#include <kstdebug.h>

#include <QDebug>
#include <QGraphicsScene>

namespace Kst {

BoxItem::BoxItem(View *parent)
    : ViewItem(parent) {
}


BoxItem::~BoxItem() {
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


ViewItem* BoxItemFactory::generateGraphics(QXmlStreamReader& xml, View *view, ViewItem *parent) {
  BoxItem *rc = 0;
  double x = 0., y = 0., w = 10., h = 10.;
  while (!xml.atEnd()) {
    if (xml.isStartElement()) {
      if (xml.name().toString() == "box") {
        Q_ASSERT(!rc);
        rc = new BoxItem(view);
        if (parent) {
          rc->setParentItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("background");
        if (!av.isNull()) {
          QBrush b = rc->brush();
          b.setColor(QColor(av.toString()));
          rc->setBrush(b);
        }
        av = attrs.value("border");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setWidthF(av.toString().toDouble());
          rc->setPen(p);
        }
        av = attrs.value("color");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setColor(QColor(av.toString()));
          rc->setPen(p);
        }
        av = attrs.value("width");
        if (!av.isNull()) {
          w = av.toString().toDouble();
        }
        av = attrs.value("height");
        if (!av.isNull()) {
          h = av.toString().toDouble();
        }
        av = attrs.value("x");
        if (!av.isNull()) {
          x = av.toString().toDouble();
        }
        av = attrs.value("y");
        if (!av.isNull()) {
          y = av.toString().toDouble();
        }
      } else {
        Q_ASSERT(rc);
        ViewItem *i = GraphicsFactory::parse(xml, view, rc);
        if (!i) {
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "box") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating box object from Kst file."), KstDebug::Warning);
        delete rc;
        return 0;
      }
    }
    xml.readNext();
  }

  rc->setRect(QRectF(QPointF(x, y), QSizeF(w, h)));
  return rc;
}

}

#include "boxitem.moc"

// vim: ts=2 sw=2 et
