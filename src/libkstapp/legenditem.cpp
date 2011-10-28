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

#define LEGENDITEMMAXWIDTH 900
#define LEGENDITEMMAXHEIGHT 100

namespace Kst {


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
  applyDialogDefaultsStroke();
  applyDialogDefaultsFill();
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

  int count = legendItems.count();
  if (count <= 0) { // no legend or box if there are no legend items
    return;
  }


  QFont font(_font);
  font.setPointSizeF(view()->scaledFontSize(_fontScale, *painter->device()));

  painter->setFont(font);

  // generate string list of relation names
  QStringList names;
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
      int i_dup = names.indexOf(label);
      if (i_dup<0) {
        names.append(label);
      } else {
        RelationPtr dup_relation = legendItems.at(i_dup);
        if (!dup_relation->yLabelInfo().file.isEmpty()) {
          names.replace(i_dup, label + " (" + dup_relation->yLabelInfo().file + ')');
        }
        if (!relation->yLabelInfo().file.isEmpty()) {
          names.append(label + " (" + relation->yLabelInfo().file + ')');
        }
      }
    }
  }

  QSize legendSize(0, 0);
  QSize titleSize(0,0);
  for (int draw = 0; draw<=1; draw++){ // do twice: once to get sizes; second to draw it.

    if (draw) {
      painter->drawRect(rect());
    }
    int x=rect().x();
    int y=rect().y();
    if (!_title.isEmpty()) {
      int pad = painter->fontMetrics().ascent()/4;
      Label::Parsed *parsed = Label::parse(_title);
      Label::RenderContext rc(painter->font(), painter);
      titleSize.setHeight(painter->fontMetrics().height()+pad);

      rc.y = rect().y() + titleSize.height()-pad;
      rc.x = qMax(rect().x()+pad, rect().x() + legendSize.width()/2 - titleSize.width()/2);
      int x0 = rc.x;

      Label::renderLabel(rc, parsed->chunk, false, true);

      titleSize.setWidth(rc.x - x0 + 3*pad);
      y+= titleSize.height();
    }
    legendSize.setWidth(0);
    legendSize.setHeight(0);
    for (int i = 0; i<count; i++) {
      RelationPtr relation = legendItems.at(i);
      QSize size;
      painter->save();
      painter->translate(x,y);
      size = paintRelation(names.at(i), relation, painter, draw);
      painter->restore();

      if (_verticalDisplay) {
        legendSize.setWidth(qMax(legendSize.width(), size.width()));
        legendSize.setHeight(legendSize.height() + size.height());
        y+=size.height();
      } else {
        legendSize.setHeight(qMax(legendSize.height(), size.height()));
        legendSize.setWidth(legendSize.width() + size.width());
        x+=size.width();
      }
    }
    if (!draw) {
      setViewRect(rect().x(), rect().y(), qMax(legendSize.width(), titleSize.width()), legendSize.height() + titleSize.height());
    }
  }



#if 0
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
      Label::renderLabel(rc, parsed->chunk, false, true);

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


  foreach(const DrawnLegendItem &item, legendPixmaps) {
    painter->drawPixmap(QPoint(x, y), item.pixmap, QRect(0, 0, item.size.width(), item.size.height()));
    if (_verticalDisplay) {
      y += item.size.height();
    } else {
      x += item.size.width();
    }
  }
#endif
}


QSize LegendItem::paintRelation(QString name, RelationPtr relation, QPainter *painter, bool draw) {
  Label::Parsed *parsed = Label::parse(name);
  parsed->chunk->attributes.color = _color;

  int fontHeight = painter->fontMetrics().height();
  int fontAscent = painter->fontMetrics().ascent();

  QSize symbol_size = relation->legendSymbolSize(painter);
  int label_width = 0;
  int paddingValue = fontHeight / 4;

  if (relation->symbolLabelOnTop()) {
    Label::RenderContext tmprc(painter->font(), painter);
    Label::renderLabel(tmprc, parsed->chunk, false, false);
    label_width = tmprc.x;
    painter->translate(paddingValue, fontHeight+paddingValue / 2);
    symbol_size.setWidth(qMax(label_width, symbol_size.width()));
  } else {
    painter->translate(paddingValue, paddingValue / 2);
  }

  if (draw) {
    relation->paintLegendSymbol(painter, symbol_size);
  }

  if (relation->symbolLabelOnTop()) {
    painter->translate((symbol_size.width()-label_width)/2, fontAscent - fontHeight);
  } else {
    painter->translate(symbol_size.width() + paddingValue, 0);
  }
  Label::RenderContext rc(painter->font(), painter);
  if (relation->symbolLabelOnTop()) {
    rc.y = 0;
  } else {
    rc.y = (symbol_size.height()+painter->fontMetrics().boundingRect('M').height())/2;
  }
  if (parsed) {
    Label::renderLabel(rc, parsed->chunk, false, draw);

    delete parsed;
    parsed = 0;
  }

  double h = symbol_size.height() + paddingValue;
  if (relation->symbolLabelOnTop()) {
    h += fontHeight;
  }
  if (relation->symbolLabelOnTop()) {
    return QSize(qMax(rc.x,(symbol_size.width())) + (paddingValue * 2), h);
  } else {
    return QSize((symbol_size.width()) + (paddingValue * 3) + rc.x, h);
  }

}


void LegendItem::save(QXmlStreamWriter &xml) {
  Q_UNUSED(xml);
}

QColor LegendItem::legendColor() const {
  return _color;
}


void LegendItem::setLegendColor(const QColor &color) {
  _color = color;
}


void LegendItem::applyDefaults() {
  _auto = _dialogDefaults->value(defaultsGroupName()+"/auto",true).toBool();

  _color = _dialogDefaults->value(defaultsGroupName()+"/color",QColor(Qt::black)).value<QColor>();

  QFont font;
  font.fromString(_dialogDefaults->value(defaultsGroupName()+"/font",font.toString()).toString());
  setLegendFont(font);

  setFontScale(_dialogDefaults->value(defaultsGroupName()+"/fontScale", 12.0).toDouble());
  _verticalDisplay = _dialogDefaults->value(defaultsGroupName()+"/verticalDisplay",true).toBool();
}

void LegendItem::setFont(const QFont &f, const QColor &c) {
  setLegendColor(c);
  setLegendFont(f);
  setFontScale(f.pointSize());
}

void LegendItem::saveAsDialogDefaults() const {
  _dialogDefaults->setValue(defaultsGroupName()+"/auto",_auto);
  _dialogDefaults->setValue(defaultsGroupName()+"/title", _title);
  _dialogDefaults->setValue(defaultsGroupName()+"/verticalDisplay", _verticalDisplay);

  QFont F = _font;
  F.setPointSize(_fontScale);
  saveDialogDefaultsFont(F, _color);
  saveDialogDefaultsPen(defaultsGroupName(), pen());
  saveDialogDefaultsBrush(defaultsGroupName(), brush());
}

void LegendItem::saveDialogDefaultsFont(const QFont &F, const QColor &C) {
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/font", QVariant(F).toString());
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/fontScale",F.pointSize());
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/color", C.name());
}

void LegendItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeStartElement("legend");
  xml.writeAttribute("auto", QVariant(_auto).toString());
  xml.writeAttribute("title", QVariant(_title).toString());
  xml.writeAttribute("font", QVariant(_font).toString());
  xml.writeAttribute("fontscale", QVariant(_fontScale).toString());
  xml.writeAttribute("color", QVariant(_color).toString());
  xml.writeAttribute("verticaldisplay", QVariant(_verticalDisplay).toString());
  ViewItem::save(xml);
  foreach (const RelationPtr &relation, _relations) {
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
    setLegendFont(font);
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


void LegendItem::setLegendFont(const QFont &font) {
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
  foreach (const RelationPtr &relation, _relations) {
    contents += QString("  %1\n").arg(relation->Name());
  }

  return i18n("Plot: %1 \nContents:\n %2").arg(Name()).arg(contents);
}

}

// vim: ts=2 sw=2 et
