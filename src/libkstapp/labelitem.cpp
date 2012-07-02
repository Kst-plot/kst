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
#include "plotitem.h"
#include "cartesianrenderitem.h"

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(View *parent, const QString& txt)
  : ViewItem(parent), _labelRc(0), _dirty(true), _text(txt), _height(0), _resized(false), _dataRelativeDimValid(false), _fixleft(false) {
  setTypeName("Label");
  setFixedSize(true);
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
  _fixleft = _dialogDefaults->value(defaultsGroupName()+"/fixLeft",true).toBool();
  applyDialogDefaultsLockPosToData();
}

void LabelItem::saveAsDialogDefaults() const {
  QFont F = _font;
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

void LabelItem::generateLabel(QPainter *p) {
  double lines = 1.0;
  if (_labelRc) {
    lines = _labelRc->lines;
    delete _labelRc;
  }

  Label::Parsed *parsed = Label::parse(_text);
  if (parsed) {
    parsed->chunk->attributes.color = _color;
    _dirty = false;
    QFont font(_font);

    if (_resized) {
      font.setPointSizeF(view()->scaledFontSize(_scale, *p->device()));
      Label::RenderContext *tmpRc = new Label::RenderContext(font, p);
      double fs_adjust = rect().height()/(tmpRc->fontHeight()*(lines+1));
      _scale *= fs_adjust;
      delete tmpRc;
    }
    font.setPointSizeF(view()->scaledFontSize(_scale, *p->device()));

    _labelRc = new Label::RenderContext(font, p);
    Label::renderLabel(*_labelRc, parsed->chunk, true, false);

    _height = _labelRc->fontHeight();
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
      if (fixLeft()) {
        setViewRect(QRectF(rect().left(), rect().bottom() - (_labelRc->lines+1) * _height,
                    _labelRc->xMax, (_labelRc->lines+1) * _height),true);
      } else {
        setViewRect(QRectF(rect().right()-_labelRc->xMax, rect().bottom() - (_labelRc->lines+1) * _height,
                    _labelRc->xMax, (_labelRc->lines+1) * _height),true);
      }
    }
    _paintTransform.reset();
    _paintTransform.translate(rect().x(), rect().y() + _labelRc->fontAscent());

    connect(_labelRc, SIGNAL(labelDirty()), this, SLOT(setDirty()));
    connect(_labelRc, SIGNAL(labelDirty()), this, SLOT(triggerUpdate()));

    delete parsed;
  }
}


void LabelItem::paint(QPainter *painter) {
  // possible optimization: make _dirty actually work to save label
  // regeneration on 'paint'.  Unlikely to be noticeable though.
  //if (_dirty || 1) {
  generateLabel(painter);
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
    xml.writeAttribute("fixleft", QVariant(_fixleft).toString());
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
    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);
    updateViewItemParent();
    updateRelativeSize();
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
    label->setFixLeft(dialog.fixLeft());
    label->setLockPosToData(dialog.lockPosToData());
    if (dialog.saveAsDefaults()) {
      label->saveAsDialogDefaults();
      _dialogDefaults->setValue(label->staticDefaultsGroupName()+"/fixLeft",dialog.fixLeft());
      _dialogDefaults->setValue(label->staticDefaultsGroupName()+"/lockPosToData",dialog.lockPosToData());
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
        av = attrs.value("fixleft");
        if (!av.isNull()) {
          rc->setFixLeft(QVariant(av.toString()).toBool());
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


void LabelItem::updateDataRelativeRect( bool force) {
  CartesianRenderItem* plot = dynamic_cast<CartesianRenderItem*>(parentViewItem());
  if (plot) {
    if ((!lockPosToData()) || force || (!_dataRelativeDimValid)) {
      _dataRelativeDimValid = ((fabs(rect().width())>1.1) && (fabs(rect().height())>1.1));
      QPointF P1 = rect().bottomLeft();
      QPointF P2 = rect().bottomRight();
      _dataRelativeRect.setTopLeft(plot->plotItem()->mapToProjection(mapToParent(P1)));
      _dataRelativeRect.setBottomRight(plot->plotItem()->mapToProjection(mapToParent(P2)));
    }
  }
}


void LabelItem::applyDataLockedDimensions() {
  PlotRenderItem *render_item = dynamic_cast<PlotRenderItem *>(parentViewItem());
  if (render_item) {
    qreal parentWidth = render_item->width();
    qreal parentHeight = render_item->height();
    qreal parentX = render_item->rect().x();
    qreal parentY = render_item->rect().y();
    qreal parentDX = render_item->plotItem()->xMax() - render_item->plotItem()->xMin();
    qreal parentDY = render_item->plotItem()->yMax() - render_item->plotItem()->yMin();

    QPointF drP1 = _dataRelativeRect.topLeft();
    QPointF drP2 = _dataRelativeRect.bottomRight();

    QPointF P1(parentX + parentWidth*(drP1.x()-render_item->plotItem()->xMin())/parentDX,
                       parentY + parentHeight*(render_item->plotItem()->yMax() - drP1.y())/parentDY);
    QPointF P2(parentX + parentWidth*(drP2.x()-render_item->plotItem()->xMin())/parentDX,
                       parentY + parentHeight*(render_item->plotItem()->yMax() - drP2.y())/parentDY);

    qreal theta = atan2(P2.y() - P1.y(), P2.x() - P1.x());
    qreal height = rect().height();
    qreal width = rect().width();

    if (_fixleft) {
      setPos(P1);
      setViewRect(0, -height, width, height);
    } else {
      setPos(P2);
      setViewRect(-width, -height, width, height);
    }
    QTransform transform;
    transform.rotateRadians(theta);

    setTransform(transform);
    updateRelativeSize();

  } else {
    qDebug() << "apply data locked dimensions called without a render item (!)";
  }
}

void LabelItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
  Q_UNUSED(oldParentRect);

  QRectF itemRect = rect();

  if (fixLeft()) {
    QPointF newBottomLeft = newParentRect.topLeft() +
        QPointF(newParentRect.width() * _parentRelativeLeft.x(),
                newParentRect.height() * _parentRelativeLeft.y());

    setPos(newBottomLeft);
    setViewRect(QRectF(0, -itemRect.height(), itemRect.width(), itemRect.height()), true);

  } else {
    QPointF newBottomRight = newParentRect.topLeft() +
        QPointF(newParentRect.width() * _parentRelativeRight.x(),
                newParentRect.height() * _parentRelativeRight.y());

    setPos(newBottomRight);
    setViewRect(QRectF(-itemRect.width(), -itemRect.height(), itemRect.width(), itemRect.height()), true);
  }

  //setViewRect(itemRect, true);
}

}

// vim: ts=2 sw=2 et
