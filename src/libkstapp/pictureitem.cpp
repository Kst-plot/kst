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

#include "pictureitem.h"

#include "debug.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QBuffer>

namespace Kst {

PictureItem::PictureItem(View *parent, const QImage &image)
  : ViewItem(parent), _image(QPixmap::fromImage(image)) {
  setName("Picture");
  setLockAspectRatio(true);
}


PictureItem::~PictureItem() {
}


void PictureItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("picture");
  QByteArray qba;
  QBuffer buffer(&qba);
  buffer.open(QIODevice::WriteOnly);
  QImage(_image).save(&buffer, "PNG"); // writes image into ba in PNG format
  xml.writeAttribute("data", qCompress(qba).toBase64());
  ViewItem::save(xml);
  xml.writeEndElement();
}


void PictureItem::paint(QPainter *painter) {
  // We can do better here.  Cache the scaled pixmap also.
  if (!_image.isNull() && rect().isValid()) {
    const qreal w = pen().widthF();
    painter->drawPixmap(rect().adjusted(w, w, -w, -w), _image, _image.rect());
  }
}


void CreatePictureCommand::createItem() {
  QString file = QFileDialog::getOpenFileName(_view, tr("Kst: Open Image"));
  if (file.isEmpty())
    return;

  _item = new PictureItem(_view, QImage(file));
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


PictureItemFactory::PictureItemFactory()
: GraphicsFactory() {
  registerFactory("picture", this);
}


PictureItemFactory::~PictureItemFactory() {
}


ViewItem* PictureItemFactory::generateGraphics(QXmlStreamReader& xml, View *view, ViewItem *parent) {
  PictureItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "picture") {
        Q_ASSERT(!rc);
        QXmlStreamAttributes attrs = xml.attributes();
        QImage loadedImage;
        QByteArray qbca = QByteArray::fromBase64(attrs.value("data").toString().toLatin1());
        loadedImage.loadFromData(qUncompress(qbca));
        rc = new PictureItem(view, loadedImage);
        if (parent) {
          rc->setParentItem(parent);
        }
        // TODO add any specialized PictureItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "picture") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating picture object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
