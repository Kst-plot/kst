/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEGENDITEM_H
#define LEGENDITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Label {
  struct Parsed;
}

namespace Kst {

class PlotItem;

class LegendItem : public ViewItem {
  Q_OBJECT
  public:
    LegendItem(PlotItem *parent);
    virtual ~LegendItem();

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    void setAutoContents(const bool autoContent);
    bool autoContents() const;

    void setVerticalDisplay(const bool vertical);
    bool verticalDisplay() const;

    QFont font() const;
    void setFont(const QFont &font);

    qreal fontScale() const;
    void setFontScale(const qreal scale);

    QString title() const;
    void setTitle(const QString &title);

    PlotItem* plot() { return _plotItem; }

  public Q_SLOTS:
    virtual void edit();

  private:
    PlotItem *_plotItem;
    bool _auto;
    QString _title;
    QFont _font;
    qreal _fontScale;
    bool _verticalDisplay;
};


class LegendItemFactory : public GraphicsFactory {
  public:
    LegendItemFactory();
    ~LegendItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
