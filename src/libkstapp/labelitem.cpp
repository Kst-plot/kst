/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
#include "labelitemdialog.h"
#include "labelcreator.h"

#include "applicationsettings.h"

#include "debug.h"
#include "dialogdefaults.h"

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(View *parent, const QString& txt)
  : ViewItem(parent), _labelRc(0), _dirty(true), _text(txt), _height(0), _resized(false) {
  setTypeName("Label");
  setFixedSize(false);
  setLockAspectRatio(true);
  setAllowedGripModes(Move | Resize | Rotate /*| Scale*/);

  applyDefaults();
}

void LabelItem::applyDefaults() {
  QFont font;
  font.fromString(_dialogDefaults->value(defaultsGroupName()+"/font",font.toString()).toString());
  _font  = font;
  _color = _dialogDefaults->value(defaultsGroupName()+"/color",QColor(Qt::black)).value<QColor>();
  _scale = _dialogDefaults->value(defaultsGroupName()+"/fontScale",12).toDouble();
}

void LabelItem::saveAsDialogDefaults() const {
  QFont F;
  F.setPointSize(_scale);
  LabelItem::saveDialogDefaultsFont(F, _color);
}

void LabelItem::saveDialogDefaultsFont(const QFont &F, const QColor &C) {
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/font", QVariant(F).toString());
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/color", C.name());
  _dialogDefaults->setValue(staticDefaultsGroupName()+"/fontScale", F.pointSize());
}

LabelItem::~LabelItem() {
  delete _labelRc;
}

void LabelItem::generateLabel() {
  double lines = 1.0;
  if (_labelRc) {
    lines = _labelRc->lines;
    delete _labelRc;
  }

  Label::Parsed *parsed = Label::parse(_text);
  if (parsed) {
    parsed->chunk->attributes.color = _color;
    _dirty = false;
    QRectF box = rect();
    QFont font(_font);
    if (_resized) {
      font.setPointSizeF(view()->viewScaledFontSize(_scale));
      QFontMetrics fm(font);
      double fs_adjust = rect().height()/(fm.height()*(lines+1));
      _scale *= fs_adjust;
    }
    font.setPointSizeF(view()->viewScaledFontSize(_scale));
    QFontMetrics fm(font);
    _paintTransform.reset();
    _paintTransform.translate(box.x(), box.y() + fm.ascent());
    _labelRc = new Label::RenderContext(font, 0);
    Label::renderLabel(*_labelRc, parsed->chunk);

    _height = fm.height();

    // Make sure we have a rect for selection, movement, etc
    if (_resized) {
      _resized = false;
      double x0 = rect().x();
      double y0 = rect().y();
      double x1 = x0 + rect().width();
      double y1 = y0 + rect().height();
      double w = _labelRc->xMax;
      double h = (_labelRc->lines+1) * _height;
      switch(_activeGrip) {
      case TopLeftGrip:
        setViewRect(QRectF(x1-w,y1-h,w,h));
        break;
      case TopRightGrip:
        setViewRect(QRectF(x0,y1-h,w,h));
        break;
      case BottomLeftGrip:
        setViewRect(QRectF(x1-w,y0,w,h));
        break;
      case BottomRightGrip:
      case NoGrip:
      default:
        setViewRect(QRectF(x0,y0,w,h));
        break;
      }
    } else {
      setViewRect(QRectF(rect().x(), rect().y(), _labelRc->xMax, (_labelRc->lines+1) * _height));
    }
    connect(_labelRc, SIGNAL(labelDirty()), this, SLOT(setDirty()));
    connect(_labelRc, SIGNAL(labelDirty()), this, SLOT(triggerUpdate()));

    delete parsed;
  }
}


void LabelItem::paint(QPainter *painter) {
  // possible optimization: make _dirty actually work to save label
  // regeneration on 'paint'.  Unlikely to be noticeable though.
  //if (_dirty || 1) {
  generateLabel();
  //}

  if (_labelRc) {
    painter->save();
    painter->setTransform(_paintTransform, true);
    Label::paintLabel(*_labelRc, painter);
    painter->restore();
  }
}


void LabelItem::triggerUpdate() {
  update();
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
  setDirty();
}


qreal LabelItem::labelScale() {
  return _scale;
}


void LabelItem::setLabelScale(const qreal scale) {
  _scale = scale;
  setDirty();
}


QColor LabelItem::labelColor() const { 
  return _color;
}


void LabelItem::setLabelColor(const QColor &color) {
  _color = color;
  setDirty();
}


void LabelItem::edit() {
  LabelItemDialog *editDialog = new LabelItemDialog(this);
  editDialog->show();
}


QFont LabelItem::labelFont() const {
  return _font;
}


void LabelItem::setLabelFont(const QFont &font) {
  _font = font;
  setDirty();
}


void LabelItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseMoveEvent(event);

  if ((gripMode() == ViewItem::Resize) && (activeGrip() != NoGrip)) {
    _resized = true;
  }

}

void LabelItem::creationPolygonChanged(View::CreationEvent event) {

  if (event == View::MouseMove) {
    if (view()->creationPolygon(View::MouseMove).size()>0) {
      const QPointF P = view()->creationPolygon(View::MouseMove).last();
      setPos(P);
      setDirty();
    }
  } else if (event == View::MouseRelease) {
    const QPointF P = mapFromScene(view()->creationPolygon(event).last());
    QRectF newRect(rect().x(), rect().y(),
                   P.x() - rect().x(),
                   P.y() - rect().y());

    if (newRect.isNull()) {
      // Special case for labels that don't need to have a size for creation to ensure proper parenting.
      newRect.setSize(QSize(1, 1));
    }

    setViewRect(newRect.normalized());

    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);
    updateViewItemParent();
    emit creationComplete();
    setDirty();
    return;
  } else if (event != View::MousePress) {
    ViewItem::creationPolygonChanged(event);
  }
}

void LabelItem::setFont(const QFont &f, const QColor &c) {
  setLabelColor(c);
  setLabelFont(f);
  setLabelScale(f.pointSize());
}

void CreateLabelCommand::createItem(QString *inText) {

  if (inText) {
    _item = new LabelItem(_view, *inText);
  } else {
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
    LabelItem *label = qobject_cast<LabelItem*>(_item);

    label->setLabelScale(dialog.labelScale());
    label->setLabelColor(dialog.labelColor());
    label->setLabelFont(dialog.labelFont());
    if (dialog.saveAsDefaults()) {
      label->saveAsDialogDefaults();
    }
  }
  _item->view()->scene()->addItem(_item);

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
          rc->setParentViewItem(parent);
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
