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

#include "buttonitem.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QPushButton>

namespace Kst {

ButtonItem::ButtonItem(View *parent) : ViewItem(parent), _pushButton(new QPushButton),_proxy(new QGraphicsProxyWidget(this)) {
  setTypeName("Button");
  setBrush(Qt::white);
  applyDialogDefaultsStroke();
  applyDialogDefaultsFill();
  applyDialogDefaultsLockPosToData();
  _pushButton->setAutoFillBackground(0);
  QPalette pal=_pushButton->palette();
  pal.setColor(_pushButton->backgroundRole(),"white");
  _pushButton->setPalette(pal);
  _pushButton->setText("");
  _proxy->setWidget(_pushButton);
  connect(_pushButton,SIGNAL(clicked()),this,SLOT(notifyScripts()));
}


ButtonItem::~ButtonItem() {
}


void ButtonItem::paint(QPainter *) {
    _proxy->setGeometry(rect());
}


void ButtonItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("button");
    xml.writeAttribute("text", _pushButton->text());
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}

void ButtonItem::setText(QString t) {
    _pushButton->setText(t);
}

void ButtonItem::addSocket(QLocalSocket* s) {
    _sockets.push_back(s);
}

void ButtonItem::notifyScripts() {
    foreach(QLocalSocket*s,_sockets) {
        s->write("clicked");
        s->flush();
    }
}

ButtonItemFactory::ButtonItemFactory()
: GraphicsFactory() {
  registerFactory("button", this);
}


ButtonItemFactory::~ButtonItemFactory() {
}


ViewItem* ButtonItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
    qDebug()<<"GG!";
  ButtonItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "button") {
        Q_ASSERT(!rc);
        QXmlStreamAttributes attrs = xml.attributes();
        rc = new ButtonItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        QStringRef av = attrs.value("text");
        if (!av.isNull()) {
          rc->_pushButton->setText(av.toString());
        }
        // Add any new specialized ButtonItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "button") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating button object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
