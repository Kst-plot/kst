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

    const QString defaultsGroupName() const {return LabelItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("label");}

    // for view item dialogs
    virtual bool hasStroke() const {return false;}
    virtual bool hasBrush() const {return false;}
    virtual bool hasFont() const {return true;}

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

    void applyDefaults();
    void saveAsDialogDefaults() const;
    static void saveDialogDefaultsFont(const QFont &F, const QColor &C);

    virtual void setFont(const QFont &f, const QColor &c);

    virtual void updateDataRelativeRect(bool force = false);
    virtual void applyDataLockedDimensions();
    virtual void updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect);

    bool fixLeft() const {return _fixleft;}
    void setFixLeft(bool fix_left) {_fixleft = fix_left;}

    virtual bool customDimensionsTab() {return true;}

  public Q_SLOTS:
    virtual void edit();
    void setDirty() { _dirty = true; }
    void triggerUpdate();

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

  private:
    void generateLabel(QPainter *p);

    Label::RenderContext *_labelRc;
    QTransform _paintTransform;
    bool _dirty;
    QString _text;
    qreal _scale;
    QColor _color;
    QFont _font;
    qreal _height;
    bool _resized;
    bool _dataRelativeDimValid;
    bool _fixleft;
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
