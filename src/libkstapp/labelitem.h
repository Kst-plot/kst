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

#ifndef LABELITEM_H
#define LABELITEM_H

#include <QGraphicsRectItem>
#include "viewitem.h"

namespace Label {
  class Parsed;
}

namespace Kst {

class LabelItem : public ViewItem, public QGraphicsRectItem {
  Q_OBJECT
  public:
    LabelItem(View *parent, const QString& labelText);
    virtual ~LabelItem();

    virtual QGraphicsItem *graphicsItem() { return this; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  private Q_SLOTS:
    void creationPolygonChanged(View::CreationEvent event);

  private:
    QPointF _originalPos;
    Label::Parsed *_parsed;
    QString _text;
};


class KST_EXPORT CreateLabelCommand : public CreateCommand {
  public:
    CreateLabelCommand() : CreateCommand(QObject::tr("Create Label")) {}
    CreateLabelCommand(View *view): CreateCommand(view, QObject::tr("Create Label")) {}
    virtual ~CreateLabelCommand() {}
    virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
