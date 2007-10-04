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
  setName("Label");

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


}

// vim: ts=2 sw=2 et
