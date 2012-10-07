/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "lineedititem.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QLineEdit>

namespace Kst {

LineEditItem::LineEditItem(View *parent) : ViewItem(parent), _lineEdit(new QLineEdit),_proxy(new QGraphicsProxyWidget(this)) {
  setTypeName("LineEdit");
  setBrush(Qt::white);
  applyDialogDefaultsStroke();
  applyDialogDefaultsFill();
  applyDialogDefaultsLockPosToData();
  _lineEdit->setAutoFillBackground(0);
  QPalette pal=_lineEdit->palette();
  pal.setColor(_lineEdit->backgroundRole(),"white");
  _lineEdit->setPalette(pal);
  _lineEdit->setText("");
  _proxy->setWidget(_lineEdit);
  connect(_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(notifyScripts()));
}


LineEditItem::~LineEditItem() {
}


void LineEditItem::paint(QPainter *) {
    _proxy->setGeometry(rect());
}


void LineEditItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("lineEdit");
    xml.writeAttribute("text", _lineEdit->text());
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}

void LineEditItem::setText(QString t) {
    _lineEdit->setText(t);
}

void LineEditItem::addSocket(QLocalSocket* s) {
    _sockets.push_back(s);
}

void LineEditItem::notifyScripts() {
    foreach(QLocalSocket*s,_sockets) {
        s->write(QString("valueSet:"+_lineEdit->text()).toLatin1());
        s->flush();
    }
}

LineEditItemFactory::LineEditItemFactory()
: GraphicsFactory() {
  registerFactory("lineEdit", this);
}


LineEditItemFactory::~LineEditItemFactory() {
}


ViewItem* LineEditItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  LineEditItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "lineEdit") {
        Q_ASSERT(!rc);
        QXmlStreamAttributes attrs = xml.attributes();
        rc = new LineEditItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        QStringRef av = attrs.value("text");
        if (!av.isNull()) {
          rc->_lineEdit->setText(av.toString());
        }
        // Add any new specialized LineEditItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "lineEdit") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating lineEdit object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
