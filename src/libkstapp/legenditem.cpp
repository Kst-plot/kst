/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   <netterfield@astro.utoronto.ca>                       *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "legenditem.h"

#include <labelparser.h>
#include "labelrenderer.h"

#include "debug.h"
#include "plotitem.h"
#include "legenditemdialog.h"
#include "objectstore.h"
#include "dialogdefaults.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

#define LEGENDITEMMAXWIDTH 400
#define LEGENDITEMMAXHEIGHT 100

namespace Kst {

struct DrawnLegendItem {
  QPixmap pixmap;
  QSize size;
};


LegendItem::LegendItem(PlotItem *parentPlot)
  : ViewItem(parentPlot->view()), _plotItem(parentPlot), _auto(true), _verticalDisplay(true) {
  setTypeName("Legend");

  _initializeShortName();

  setFixedSize(true);
  setAllowedGripModes(Move /*| Resize*/ /*| Rotate*/ /*| Scale*/);

  setViewRect(0.0, 0.0, 0.0, 0.0);
  view()->scene()->addItem(this);
  setParentViewItem(_plotItem->renderItem());

  QPointF origin = QPointF(_plotItem->plotRect().width() * 0.15, _plotItem->plotRect().height() * 0.15);
  setPos(origin);

  applyDefaults();
}

void LegendItem::_initializeShortName() {
  _shortName = 'L'+QString::number(_lnum);
  if (_lnum>max_lnum)
    max_lnum = _lnum;
  _lnum++;
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
  font.setPointSizeF(view()->defaultFont(_fontScale).pointSizeF());

  // generate string list of relation names
  QStringList names;
  int count = legendItems.count();
  bool allAuto = true;
  bool sameX = true;
  bool sameYUnits = true;
  LabelInfo label_info = legendItems.at(0)->xLabelInfo();
  QString yUnits =  legendItems.at(0)->yLabelInfo().units;

  for (int i = 0; i<count; i++) {
    RelationPtr relation = legendItems.at(i);
    if (relation->descriptiveNameIsManual()) {
      allAuto = false;
    }
    if (relation->xLabelInfo() != label_info) {
      sameX = false;
    }
    // sameYUnits is false if any non empty units are defined differently.
    if (yUnits.isEmpty()) {
      yUnits = relation->yLabelInfo().units;
    } else if (relation->yLabelInfo().units != yUnits) {
      if (!relation->yLabelInfo().units.isEmpty()) {
        sameYUnits = false;
      }
    }
  }

  if (!allAuto) {
    for (int i = 0; i<count; i++) {
      names.append(legendItems.at(i)->descriptiveName());
    }
  } else {
    for (int i = 0; i<count; i++) {
      RelationPtr relation = legendItems.at(i);
      QString label = relation->titleInfo().singleRenderItemLabel();
      if (label.isEmpty()) {
        label_info = relation->yLabelInfo();
        QString y_label = label_info.name;
        if (!sameYUnits) {
          if (!label_info.units.isEmpty()) {
            y_label = i18n("%1 \\[%2\\]").arg(y_label).arg(label_info.units);
          }
        }
        if (!y_label.isEmpty()) {
          LabelInfo xlabel_info = relation->xLabelInfo();
          if (!sameX) {
            label = i18n("%1 vs %2").arg(y_label).arg(xlabel_info.name);
          } else if (xlabel_info.quantity.isEmpty()) {
            label = y_label;
          } else if (xlabel_info.quantity != xlabel_info.name) {
            label = i18n("%1 vs %2").arg(y_label).arg(xlabel_info.name);
          } else {
            label = y_label;
          }
        } else {
          label = relation->descriptiveName();
        }
      }
      names.append(label);
    }
  }

  for (int i = 0; i<count; i++) {
    RelationPtr relation = legendItems.at(i);
    DrawnLegendItem item;
    item.pixmap = QPixmap(LEGENDITEMMAXWIDTH, LEGENDITEMMAXHEIGHT);
    item.size = paintRelation(names.at(i), relation, &item.pixmap, font);

    if (_verticalDisplay) {
      legendSize.setWidth(qMax(legendSize.width(), item.size.width()));
      legendSize.setHeight(legendSize.height() + item.size.height());
    } else {
      legendSize.setHeight(qMax(legendSize.height(), item.size.height()));
      legendSize.setWidth(legendSize.width() + item.size.width());
    }

    legendPixmaps.append(item);
  }

  int x = rect().left();
  int y = rect().top();

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
      Label::renderLabel(rc, parsed->chunk, false);

      int startPoint = qMax(0, (legendSize.width() / 2) - (rc.x / 2));
      int paddingValue = fm.height() / 4;
    
      setViewRect(viewRect().x(), viewRect().y(), qMax(rc.x, legendSize.width()), rc.y + legendSize.height() + paddingValue * 3);
      painter->drawRect(rect());

      painter->drawPixmap(QPoint(x + startPoint, y + paddingValue), pixmap, QRect(0, 0, rc.x, fm.height()));
      painter->restore();
      y += fm.height() + (paddingValue *2);
      delete parsed;
      parsed = 0;
    }
  } else {
    // No Title
    setViewRect(viewRect().x(), viewRect().y(), legendSize.width(), legendSize.height());
    painter->drawRect(rect());
  }


  foreach(DrawnLegendItem item, legendPixmaps) {
    painter->drawPixmap(QPoint(x, y), item.pixmap, QRect(0, 0, item.size.width(), item.size.height()));
    if (_verticalDisplay) {
      y += item.size.height();
    } else {
      x += item.size.width();
    }
  }

  painter->restore();
}


QSize LegendItem::paintRelation(QString name, RelationPtr relation, QPixmap *pixmap, const QFont &font) {
  Label::Parsed *parsed = Label::parse(name);

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
    Label::renderLabel(rc, parsed->chunk, false);
    delete parsed;
    parsed = 0;
  }

  return QSize((4 * fm.ascent()) + (paddingValue * 3) + rc.x, fm.height() + paddingValue);
}


void LegendItem::save(QXmlStreamWriter &xml) {
  Q_UNUSED(xml);
}

void LegendItem::applyDefaults() {
  _auto = _dialogDefaults->value("legend/auto",true).toBool();

  QFont font;
  font.fromString(_dialogDefaults->value("legend/font",font.toString()).toString());
  setFont(font);

  setFontScale(_dialogDefaults->value("legend/fontScale", 0.0).toDouble());
  _verticalDisplay = _dialogDefaults->value("legend/verticalDisplay",true).toBool();

   // set the pen
  QPen pen;
  pen.setStyle((Qt::PenStyle)_dialogDefaults->value("legend/strokeStyle", 1).toInt());
  pen.setWidthF(_dialogDefaults->value("legend/strokeWidth",0).toDouble());
  pen.setJoinStyle((Qt::PenJoinStyle)_dialogDefaults->value("legend/strokeJoinStyle",64).toInt());
  pen.setCapStyle((Qt::PenCapStyle)_dialogDefaults->value("legend/strokeCapStyle",16).toInt());
  QBrush brush;
  QColor color = _dialogDefaults->value("legend/strokeBrushColor",QColor(Qt::black)).value<QColor>();
  brush.setColor(color);
  brush.setStyle((Qt::BrushStyle)_dialogDefaults->value("legend/strokeBrushStyle",1).toInt());
  pen.setBrush(brush);
  setPen(pen);

  //set the brush
  bool useGradient = _dialogDefaults->value("legend/fillBrushUseGradient", false).toBool();
  if (useGradient) {
    QStringList stopInfo =
        _dialogDefaults->value("legend/fillBrushGradient", "0,#000000,1,#ffffff,").
        toString().split(',', QString::SkipEmptyParts);
    QLinearGradient gradient(1,0,0,0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    for (int i = 0; i < stopInfo.size(); i+=2) {
      gradient.setColorAt(stopInfo.at(i).toDouble(), QColor(stopInfo.at(i+1)));
    }
    brush = QBrush(gradient);
  } else {
    color = _dialogDefaults->value("legend/fillBrushColor",QColor(Qt::white)).value<QColor>();
    brush.setColor(color);
    brush.setStyle((Qt::BrushStyle)_dialogDefaults->value("legend/fillBrushStyle",1).toInt());
  }
  setBrush(brush);

}

void LegendItem::saveAsDialogDefaults() const {
  _dialogDefaults->setValue("legend/auto",_auto);
  _dialogDefaults->setValue("legend/title", _title);
  _dialogDefaults->setValue("legend/font", QVariant(_font).toString());
  _dialogDefaults->setValue("legend/fontScale",_fontScale);
  _dialogDefaults->setValue("legend/verticalDisplay", _verticalDisplay);

  _dialogDefaults->setValue("legend/strokeStyle", QVariant(pen().style()).toString());
  _dialogDefaults->setValue("legend/strokeWidth", QVariant(pen().widthF()).toString());
  _dialogDefaults->setValue("legend/strokeCap", QVariant(pen().capStyle()).toString());
  _dialogDefaults->setValue("legend/strokeJoinStyle", QVariant(pen().joinStyle()).toString());
  _dialogDefaults->setValue("legend/strokeBrushColor", pen().brush().color().name());
  _dialogDefaults->setValue("legend/strokeBrushStyle", QVariant(pen().brush().style()).toString());

  QBrush b = brush();
  _dialogDefaults->setValue("legend/fillBrushColor", b.color().name());
  _dialogDefaults->setValue("legend/fillBrushStyle", QVariant(b.style()).toString());
  _dialogDefaults->setValue("legend/fillBrushUseGradient", QVariant(bool(b.gradient())).toString());
  if (b.gradient()) {
    QString stopList;
    foreach(QGradientStop stop, b.gradient()->stops()) {
      qreal point = (qreal)stop.first;
      QColor color = (QColor)stop.second;

      stopList += QString::number(point);
      stopList += ',';
      stopList += color.name();
      stopList += ',';
    }
     _dialogDefaults->setValue("legend/fillBrushGradient", stopList);
   }

  // Save legend defaults here
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
  LegendItemDialog *editDialog = new LegendItemDialog(this);
  editDialog->show();
}


void LegendItem::remove() {
  if (_plotItem) {
    _plotItem->setShowLegend(false);
  }
  ViewItem::remove();
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

QString LegendItem::_automaticDescriptiveName() const {

  QString name = i18n("Empty Legend");
  if (_auto) {
    name = _plotItem->descriptiveName();
  } else if (_relations.size()>0) {
    name = _relations.at(0)->descriptiveName();
    if (_relations.size()>1) {
      name += ", ...";
    }
  }
  //qDebug() << "a desc name called: " << name << " relation.length: " << _relations.length();
  return name;
}

QString LegendItem::descriptionTip() const {
  QString contents;
  foreach (Relation *relation, _relations) {
    contents += QString("  %1\n").arg(relation->Name());
  }

  return i18n("Plot: %1 \nContents:\n %2").arg(Name()).arg(contents);
}

}

// vim: ts=2 sw=2 et
