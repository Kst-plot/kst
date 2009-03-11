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

#include "viewitemzorder.h"
#include "debug.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QBuffer>

namespace Kst {

PictureItem::PictureItem(View *parent, const QImage &image)
  : ViewItem(parent) {
  if (!image.isNull()) {
    _image = QPixmap::fromImage(image);
  }
  setTypeName("Picture");
  setZValue(PICTURE_ZVALUE);
  setLockAspectRatio(true);
  setLockAspectRatioFixed(true);
}


PictureItem::~PictureItem() {
}


void PictureItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("picture");
    ViewItem::save(xml);
    xml.writeStartElement("data");
    QByteArray qba;
    QBuffer buffer(&qba);
    buffer.open(QIODevice::WriteOnly);
    _image.toImage().save(&buffer, "PNG"); // writes image into ba in PNG format
    xml.writeCharacters(qCompress(qba).toBase64());
    xml.writeEndElement();
    xml.writeEndElement();
  }
}


void PictureItem::setImage(const QImage &image)
{
  _image = QPixmap::fromImage(image);
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


ViewItem* PictureItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  PictureItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "picture") {
        Q_ASSERT(!rc);
        rc = new PictureItem(view);
        if (parent) {
          rc->setParent(parent);
        }
        // Add any new specialized PictureItem Properties here.
      } else if (xml.name().toString() == "data") {
        Q_ASSERT(rc);
        xml.readNext();
        QImage loadedImage;
        QByteArray qbca = QByteArray::fromBase64(xml.text().toString().toLatin1());
        loadedImage.loadFromData(qUncompress(qbca));
        rc->setImage(loadedImage);
        xml.readNext();
        if (!xml.isEndElement() || (xml.name().toString() != "data")) {
          validTag = false;
        }
        xml.readNext();
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
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
