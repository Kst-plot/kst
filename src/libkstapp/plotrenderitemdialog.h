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

#ifndef PLOTRENDERITEMDIALOG_H
#define PLOTRENDERITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class ContentTab;
class PlotMarkersTab;
class AxisTab;
class LabelTab;
class ObjectStore;
class PlotRenderItem;

class KST_EXPORT PlotRenderItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent = 0);
    virtual ~PlotRenderItemDialog();

  private Q_SLOTS:
    void contentChanged();
    void relationChanged();
    void xAxisChanged();
    void yAxisChanged();
    void labelsChanged();

  private:
    void setupContent();
    void setupAxis();
    void setupLabels();
    void addRelations();
    void updateRelations();

  private:
    ContentTab *_contentTab;
    AxisTab *_xAxisTab;
    AxisTab *_yAxisTab;
    LabelTab *_labelTab;
    PlotMarkersTab* _plotMarkersTab;
    PlotRenderItem* _plotItem;
    QList<DialogPage*> _relationPages;

    ObjectStore* _store;
};

}

#endif

// vim: ts=2 sw=2 et
