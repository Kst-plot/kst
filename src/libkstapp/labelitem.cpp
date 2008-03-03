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

#include "labelitem.h"

#include <labelparser.h>
#include "labelrenderer.h"
#include "viewitemzorder.h"

#include "debug.h"

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(View *parent, const QString& txt)
  : ViewItem(parent), _parsed(0), _text(txt) {
  setName("Label");
  setZValue(LABEL_ZVALUE);

  setAllowedGripModes(Move /*| Resize*/ | Rotate /*| Scale*/);
}


LabelItem::~LabelItem() {
  delete _parsed;
  _parsed = 0;
}


void LabelItem::paint(QPainter *painter) {
  if (!_parsed) {
    _parsed = Label::parse(_text);
  }

  // We can do better here. - caching
  if (_parsed) {
    const qreal w = pen().widthF();
    painter->save();
    QRectF box = rect().adjusted(w, w, -w, -w);
    QFont font;
    font.setPointSize(16);
    QFontMetrics fm(font);
    painter->translate(QPointF(box.x(), box.y() + fm.ascent()));
    Label::RenderContext rc(font.family(), font.pointSize(), painter);
    Label::renderLabel(rc, _parsed->chunk);

    // Make sure we have a rect for selection, movement, etc
    setViewRect(QRectF(box.x(), box.y(), rc.x, fm.height()));

    painter->restore();
  }
}


void LabelItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("label");
  xml.writeAttribute("text", _text);
  ViewItem::save(xml);
  xml.writeEndElement();
}


void CreateLabelCommand::createItem() {
  bool ok;
  QString text = QInputDialog::getText(_view, tr("Kst: Create Label"), tr("Label:"), QLineEdit::Normal, QString::null, &ok);
  if (!ok || text.isEmpty()) {
    return;
  }

  _item = new LabelItem(_view, text);
  _view->setCursor(Qt::IBeamCursor);

  CreateCommand::createItem();
}


LabelItemFactory::LabelItemFactory()
: GraphicsFactory() {
  registerFactory("label", this);
}


LabelItemFactory::~LabelItemFactory() {
}


ViewItem* LabelItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  LabelItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "label") {
      QXmlStreamAttributes attrs = xml.attributes();
      QStringRef av;
      av = attrs.value("text");
      if (!av.isNull()) {
        Q_ASSERT(!rc);
        rc = new LabelItem(view, av.toString());
        if (parent) {
          rc->setParent(parent);
         // TODO add any specialized LabelItem Properties here.
          }
        }
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "label") {
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
