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
#include "labelitemdialog.h"
#include "labelcreator.h"

#include "applicationsettings.h"

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

  setFixedSize(true);
  setAllowedGripModes(Move /*| Resize*/ | Rotate /*| Scale*/);
  _font = parentView()->defaultFont();
  _color = ApplicationSettings::self()->defaultFontColor();
  _scale = ApplicationSettings::self()->defaultFontScale();
}


LabelItem::~LabelItem() {
  delete _parsed;
  _parsed = 0;
}


void LabelItem::paint(QPainter *painter) {
  if (!_parsed) {
    _parsed = Label::parse(_text);
    _parsed->chunk->attributes.color = _color;
  }

  // We can do better here. - caching
  if (_parsed) {
    painter->save();
    QRectF box = rect();
    QFont font(_font);
    font.setPixelSize(parentView()->defaultFont(_scale).pixelSize());
    QFontMetrics fm(font);
    painter->translate(QPointF(box.x(), box.y() + fm.ascent()));
    Label::RenderContext rc(font, painter);
    Label::renderLabel(rc, _parsed->chunk);

    // Make sure we have a rect for selection, movement, etc
    setViewRect(QRectF(box.x(), box.y(), rc.x, fm.height()));

    painter->restore();
  }
}


void LabelItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("label");
    xml.writeAttribute("text", _text);
    xml.writeAttribute("scale", QVariant(_scale).toString());
    xml.writeAttribute("color", QVariant(_color).toString());
    xml.writeAttribute("font", QVariant(_font).toString());
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


QString LabelItem::labelText() {
  return _text;
}


void LabelItem::setLabelText(const QString &text) {
  _text = text;
  delete _parsed;
  _parsed = 0;
}


qreal LabelItem::labelScale() {
  return _scale;
}


void LabelItem::setLabelScale(const qreal scale) {
  _scale = scale;
}


QColor LabelItem::labelColor() const { 
  return _color;
}


void LabelItem::setLabelColor(const QColor &color) {
  _color = color;
}


void LabelItem::edit() {
  LabelItemDialog editDialog(this);
  editDialog.exec();
}


QFont LabelItem::labelFont() const {
  return _font;
}


void LabelItem::setLabelFont(const QFont &font) {
  _font = font;
}


void CreateLabelCommand::createItem() {
  bool ok = false;
  QString text;
  LabelCreator dialog;
  if (dialog.exec() == QDialog::Accepted) {
    text = dialog.labelText();
    ok = true;
  }

  if (!ok || text.isEmpty()) {
    return;
  }

  _item = new LabelItem(_view, text);
  LabelItem *label = qgraphicsitem_cast<LabelItem*>(_item);
  label->setLabelScale(dialog.labelScale());
  label->setLabelColor(dialog.labelColor());
  label->setLabelFont(dialog.labelFont());

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
      if (!rc && xml.name().toString() == "label") {
      QXmlStreamAttributes attrs = xml.attributes();
      QStringRef av;
      av = attrs.value("text");
      if (!av.isNull()) {
        Q_ASSERT(!rc);
        rc = new LabelItem(view, av.toString());
        if (parent) {
          rc->setParent(parent);
         // Add any new specialized LabelItem Properties here.
          }
        }
        av = attrs.value("scale");
        if (!av.isNull()) {
          rc->setLabelScale(QVariant(av.toString()).toInt());
        }
        av = attrs.value("color");
        if (!av.isNull()) {
            rc->setLabelColor(QColor(av.toString()));
        }
        av = attrs.value("font");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setLabelFont(font);
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
