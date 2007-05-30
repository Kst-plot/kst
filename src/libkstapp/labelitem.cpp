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

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(View *parent, const QString& txt)
    : ViewItem(parent), _parsed(0), _text(txt) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  parent->setMouseMode(View::Create);
  parent->setCursor(Qt::IBeamCursor);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(deleteLater()));

  connect(parent, SIGNAL(creationPolygonChanged(View::CreationEvent)),
          this, SLOT(creationPolygonChanged(View::CreationEvent)));
}


LabelItem::~LabelItem() {
  delete _parsed;
  _parsed = 0;
}


void LabelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  if (!_parsed) {
    _parsed = Label::parse(_text);
  }

  // We can do better here. - caching
  if (_parsed) {
    const qreal w = pen().widthF();
    painter->save();
    QRect box = rect().adjusted(w, w, -w, -w).toRect();
    QFont font;
    font.setPointSize(16);
    QFontMetrics fm(font);
    painter->translate(QPoint(box.x(), box.y() + fm.ascent() + fm.descent() + 1));
    Label::RenderContext rc(font.family(), 16, painter);
    Label::renderLabel(rc, _parsed->chunk);
    painter->restore();
  }
  QBrush b = brush();
  setBrush(Qt::NoBrush);
  QGraphicsRectItem::paint(painter, option, widget);
  setBrush(b);
}


void LabelItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mousePressEvent(event);
  _originalPos = pos();
}


void LabelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mouseReleaseEvent(event);

  QPointF newPos = pos();
  if (_originalPos != newPos)
    new MoveCommand(this, _originalPos, newPos);
}


void LabelItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setRect(poly.first().x(), poly.first().y(), poly.last().x() - poly.first().x(), poly.last().y() - poly.first().y());
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    setRect(rect().x(), rect().y(), poly.last().x() - rect().x(), poly.last().y() - rect().y());
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    setRect(rect().x(), rect().y(), poly.last().x() - rect().x(), poly.last().y() - rect().y());

#ifdef DEBUG_GEOMETRY
    debugGeometry();
#endif

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    emit creationComplete();
    return;
  }
}


void CreateLabelCommand::createItem() {
  bool ok;
  QString text = QInputDialog::getText(_view, tr("Kst: Create Label"), tr("Label:"), QLineEdit::Normal, QString::null, &ok);
  if (!ok || text.isEmpty()) {
    return;
  }

  _item = new LabelItem(_view, text);
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));

  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));

}


}

#include "labelitem.moc"

// vim: ts=2 sw=2 et
