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

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(View *parent)
    : ViewItem(parent) {
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
}


void LabelItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsSimpleTextItem::mousePressEvent(event);
  _originalPos = pos();
}


void LabelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsSimpleTextItem::mouseReleaseEvent(event);

  QPointF newPos = pos();
  if (_originalPos != newPos)
    new MoveCommand(this, _originalPos, newPos);
}


void LabelItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {

    bool ok;
    QString text = QInputDialog::getText(parentView(), QObject::tr("label"),
                                         QObject::tr("label:"), QLineEdit::Normal,
                                         QString::null, &ok);
    if (!ok || text.isEmpty()) {
      //This will delete...
      parentView()->setMouseMode(View::Default);
      return;
    }

    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setText(text);
    setPos(poly[0]);
    parentView()->scene()->addItem(this);
    setZValue(1);

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
  _item = new LabelItem(_view);
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));

  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));

}


}

#include "labelitem.moc"

// vim: ts=2 sw=2 et
