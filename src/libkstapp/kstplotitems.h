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

#ifndef KSTPLOTITEMS_H
#define KSTPLOTITEMS_H

#include <QObject>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

#include "kst_export.h"

#include "kstplotview.h" //forward declare, but enums??

class QGraphicsItem;

class KST_EXPORT KstPlotItem : public QObject
{
  Q_OBJECT
public:
  KstPlotItem(KstPlotView *parent);
  virtual ~KstPlotItem();

  KstPlotView *parentView() const;

  virtual QGraphicsItem *graphicsItem() = 0;

public Q_SLOTS:
  void updateAspectFromGeometry();
  void updateGeometry();

private:
  QPointF _aspectPos;   // In % from 0.0 to 100.0
  QSizeF _aspectSize;   // In % from 0.0 to 100.0
};

class LabelItem : public KstPlotItem, public QGraphicsSimpleTextItem
{
  Q_OBJECT
public:
  LabelItem(const QString &text, KstPlotView *parent);
  virtual ~LabelItem();

  virtual QGraphicsItem *graphicsItem() { return this; }
};

class LineItem : public KstPlotItem, public QGraphicsLineItem
{
  Q_OBJECT
public:
  LineItem(KstPlotView *parent);
  virtual ~LineItem();

  virtual QGraphicsItem *graphicsItem() { return this; }

private Q_SLOTS:
  void creationPolygonChanged(KstPlotView::CreationEvent event);
};

#endif

// vim: ts=2 sw=2 et
