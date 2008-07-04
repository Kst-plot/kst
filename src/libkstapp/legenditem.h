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

    void setAutoLegend(const bool autoLegend);
    bool autoLegend() const;

  public Q_SLOTS:
    virtual void edit();

  private:
    PlotItem *_plotItem;
    bool _auto;
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
