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

#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QObject>
#include "kst_export.h"
#include "kstplotview.h" //forward declare, but enums??

// #define DEBUG_GEOMETRY 1

class QGraphicsItem;

namespace Kst {

class KST_EXPORT ViewItem : public QObject
{
  Q_OBJECT
public:
  ViewItem(KstPlotView *parent);
  virtual ~ViewItem();

  KstPlotView *parentView() const;

  virtual QGraphicsItem *graphicsItem() = 0;

Q_SIGNALS:
  void creationComplete();

#ifdef DEBUG_GEOMETRY
protected:
  void debugGeometry();

private:
  QGraphicsRectItem *_rectItem;
#endif
};

}

#endif

// vim: ts=2 sw=2 et
