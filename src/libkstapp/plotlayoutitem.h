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

#ifndef PLOTLAYOUTITEM_H
#define PLOTLAYOUTITEM_H

#include "viewitem.h"

namespace Kst {

class PlotLayoutItem : public ViewItem
{
  Q_OBJECT
  public:
    PlotLayoutItem(View *parent);
    virtual ~PlotLayoutItem();

    qreal width() const { return viewRect().normalized().width(); }
    qreal height() const { return viewRect().normalized().height(); }

    Qt::Orientation orientation() const { return _orientation; }

  public Q_SLOTS:
    void updateGeometry();

  private:
    Qt::Orientation _orientation;
};

}

#endif

// vim: ts=2 sw=2 et
