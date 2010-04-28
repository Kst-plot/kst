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

#include "viewitem.h"
#include "labelrenderer.h"
#include "graphicsfactory.h"

namespace Label {
  struct Parsed;
}

namespace Kst {

class LabelItem : public ViewItem {
  Q_OBJECT
  public:
    LabelItem(View *parent, const QString& labelText);
    virtual ~LabelItem();

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    QString labelText();
    void setLabelText(const QString &text);

    qreal labelScale();
    void setLabelScale(const qreal scale);

    QColor labelColor() const;
    void setLabelColor(const QColor &color);

    QFont labelFont() const;
    void setLabelFont(const QFont &font);

  public Q_SLOTS:
    virtual void edit();
    void setDirty() { _dirty = true; };
    void triggerUpdate();

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);

  private:
    void generateLabel();

    Label::RenderContext *_labelRc;
    QTransform _paintTransform;
    bool _dirty;
    QString _text;
    qreal _scale;
    QColor _color;
    QFont _font;
    qreal _height;
};


class CreateLabelCommand : public CreateCommand {
  public:
    CreateLabelCommand() : CreateCommand(QObject::tr("Create Label")) {}
    CreateLabelCommand(View *view): CreateCommand(view, QObject::tr("Create Label")) {}
    virtual ~CreateLabelCommand() {}
    virtual void createItem(QString *inText = 0);
};

class LabelItemFactory : public GraphicsFactory {
  public:
    LabelItemFactory();
    ~LabelItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
