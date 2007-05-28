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

#include "kstplotcommands.h"

#include <QDebug>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LabelItem::LabelItem(KstPlotView *parent)
    : ViewItem(parent) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  parent->setMouseMode(KstPlotView::Create);
  parent->setCursor(Qt::IBeamCursor);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(deleteLater()));

  connect(parent, SIGNAL(creationPolygonChanged(KstPlotView::CreationEvent)),
          this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
}


LabelItem::~LabelItem() {
}


QVariant LabelItem::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange && scene()) {
    QPointF originalPos = pos();
    QPointF newPos = value.toPointF();
    // FIXME this is too greedy as it produces too many undo commands.
    // Ideally, we'd only record the move from right before the item
    // becomes the mouse grabber to right after.
    new MoveCommand(this, originalPos, newPos);
  }
  return QGraphicsItem::itemChange(change, value);
}


void LabelItem::creationPolygonChanged(KstPlotView::CreationEvent event) {
  if (event == KstPlotView::MousePress) {

    bool ok;
    QString text = QInputDialog::getText(parentView(), QObject::tr("label"),
                                         QObject::tr("label:"), QLineEdit::Normal,
                                         QString::null, &ok);
    if (!ok || text.isEmpty()) {
      //This will delete...
      parentView()->setMouseMode(KstPlotView::Default);
      return;
    }

    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(KstPlotView::MousePress));
    setText(text);
    setPos(poly[0]);
    parentView()->scene()->addItem(this);
    setZValue(1);

#ifdef DEBUG_GEOMETRY
    debugGeometry();
#endif

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
    parentView()->setMouseMode(KstPlotView::Default);
    emit creationComplete();
    return;
  }
}

}

#include "labelitem.moc"

// vim: ts=2 sw=2 et
