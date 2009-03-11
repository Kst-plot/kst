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
#include "objectstore.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

#define LEGENDITEMMAXWIDTH 400
#define LEGENDITEMMAXHEIGHT 100

namespace Kst {

struct DrawnLegendItem {
  QPixmap *pixmap;
  QSize size;
};


LegendItem::LegendItem(PlotItem *parent)
  : ViewItem(parent->parentView()), _plotItem(parent), _auto(true), _fontScale(0.0), _verticalDisplay(true) {
  setTypeName("Legend");
  setZValue(LEGEND_ZVALUE);

  setAllowedGripModes(Move /*| Resize*/ /*| Rotate*/ /*| Scale*/);

  setViewRect(0.0, 0.0, 0.0, 0.0);
  parentView()->scene()->addItem(this);
  setParent(_plotItem);

  QPointF origin = QPointF(parent->width() / 10, parent->height() / 5);
  setPos(origin);
}


LegendItem::~LegendItem() {
}


void LegendItem::paint(QPainter *painter) {
  if (!isVisible()) {
    return;
  }

  RelationList legendItems;
  if (_auto) {
    legendItems = plot()->renderItem(PlotRenderItem::Cartesian)->relationList();
  } else {
    legendItems = _relations;
  }

  QList<DrawnLegendItem> legendPixmaps;
  QSize legendSize(0, 0);

  QFont font(_font);
  font.setPixelSize(parentView()->defaultFont(_fontScale).pixelSize());

  foreach(RelationPtr relation, legendItems) {
    DrawnLegendItem item;
    item.pixmap = new QPixmap(LEGENDITEMMAXWIDTH, LEGENDITEMMAXHEIGHT);
    item.size = paintRelation(relation, item.pixmap, font);

    if (_verticalDisplay) {
      legendSize.setWidth(qMax(legendSize.width(), item.size.width()));
      legendSize.setHeight(legendSize.height() + item.size.height());
    } else {
      legendSize.setHeight(qMax(legendSize.height(), item.size.height()));
      legendSize.setWidth(legendSize.width() + item.size.width());
    }

    legendPixmaps.append(item);
  }

  int x = 0, y = 0;

  painter->save();

  if (!_title.isEmpty()) {
    // Paint the title
    Label::Parsed *parsed = Label::parse(_title);

    if (parsed) {
      painter->save();

      QPixmap pixmap(400, 100);
      pixmap.fill(Qt::transparent);
      QPainter pixmapPainter(&pixmap);

      Label::RenderContext rc(font, &pixmapPainter);
      QFontMetrics fm(font);
      rc.y = fm.ascent();
      Label::renderLabel(rc, parsed->chunk);

      int startPoint = qMax(0, (legendSize.width() / 2) - (rc.x / 2));
      int paddingValue = fm.height() / 4;

      setViewRect(viewRect().x(), viewRect().y(), qMax(rc.x, legendSize.width()), rc.y + legendSize.height() + paddingValue * 3);

      painter->drawPixmap(QPoint(x + startPoint, y + paddingValue), pixmap, QRect(0, 0, rc.x, fm.height()));
      painter->restore();
      y += fm.height() + (paddingValue *2);
      delete parsed;
      parsed = 0;
    }
  } else {
    // No Title
    setViewRect(viewRect().x(), viewRect().y(), legendSize.width(), legendSize.height());
  }

  QRectF box = rect();
  painter->drawRect(rect());

  foreach(DrawnLegendItem item, legendPixmaps) {
    painter->drawPixmap(QPoint(x, y), *(item.pixmap), QRect(0, 0, item.size.width(), item.size.height()));
    if (_verticalDisplay) {
      y += item.size.height();
    } else {
      x += item.size.width();
    }
  }

  painter->restore();
}


QSize LegendItem::paintRelation(RelationPtr relation, QPixmap *pixmap, const QFont &font) {
  Label::Parsed *parsed = Label::parse(relation->descriptiveName());

  pixmap->fill(Qt::transparent);

  QPainter pixmapPainter(pixmap);
  QFontMetrics fm(font);

  int paddingValue = fm.height() / 4;
  pixmapPainter.translate(paddingValue, paddingValue / 2);
  relation->paintLegendSymbol(&pixmapPainter, QRect(0, 0, 4 * fm.ascent(), fm.height()));

  pixmapPainter.translate(4 * fm.ascent() + paddingValue, 0);

  Label::RenderContext rc(font, &pixmapPainter);
  rc.y = fm.ascent();

  if (parsed) {
    Label::renderLabel(rc, parsed->chunk);
    delete parsed;
    parsed = 0;
  }

  return QSize((4 * fm.ascent()) + (paddingValue * 3) + rc.x, fm.height() + paddingValue);
}


void LegendItem::save(QXmlStreamWriter &xml) {
  Q_UNUSED(xml);
}


void LegendItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeStartElement("legend");
  xml.writeAttribute("auto", QVariant(_auto).toString());
  xml.writeAttribute("title", QVariant(_title).toString());
  xml.writeAttribute("font", QVariant(_font).toString());
  xml.writeAttribute("fontscale", QVariant(_fontScale).toString());
  xml.writeAttribute("verticaldisplay", QVariant(_verticalDisplay).toString());
  ViewItem::save(xml);
  foreach (RelationPtr relation, _relations) {
    xml.writeStartElement("relation");
    xml.writeAttribute("tag", relation->Name());
    xml.writeEndElement();
  }
  xml.writeEndElement();
}


bool LegendItem::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av;
  av = attrs.value("auto");
  if (!av.isNull()) {
    setAutoContents(QVariant(av.toString()).toBool());
  }
  av = attrs.value("title");
  if (!av.isNull()) {
    setTitle(av.toString());
  }
  av = attrs.value("font");
  if (!av.isNull()) {
    QFont font;
    font.fromString(av.toString());
    setFont(font);
  }
  av = attrs.value("fontscale");
  if (!av.isNull()) {
    setFontScale(QVariant(av.toString()).toDouble());
  }
  av = attrs.value("verticaldisplay");
  if (!av.isNull()) {
    setVerticalDisplay(QVariant(av.toString()).toBool());
  }
  QString expectedEnd;
  while (!(xml.isEndElement() && (xml.name().toString() == primaryTag))) {
   if (xml.isStartElement()) {
    if (xml.name().toString() == "relation") {
      expectedEnd = xml.name().toString();
      attrs = xml.attributes();
      QString tagName = attrs.value("tag").toString();
      RelationPtr relation = kst_cast<Relation>(store->retrieveObject(tagName));
      if (relation) {
        _relations.append(relation);
      }
    } else {
      parse(xml, validTag);
    }
   } else if (xml.isEndElement()) {
      if (xml.name().toString() != expectedEnd) {
        validTag = false;
        break;
      }
    }
    xml.readNext();
  }

  return validTag;
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

}

// vim: ts=2 sw=2 et
