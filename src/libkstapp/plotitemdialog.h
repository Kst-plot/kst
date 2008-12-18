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

#ifndef PLOTITEMDIALOG_H
#define PLOTITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class ContentTab;
class PlotMarkersTab;
class AxisTab;
class MarkersTab;
class LabelTab;
class ObjectStore;
class PlotItem;
class OverrideLabelTab;
class DialogPageTab;

class KST_EXPORT PlotItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    PlotItemDialog(PlotItem *item, QWidget *parent = 0);
    virtual ~PlotItemDialog();

  private Q_SLOTS:
    void contentChanged();
    void relationChanged();
    void xAxisChanged();
    void yAxisChanged();
    void labelsChanged();
    void xAxisPlotMarkersChanged();
    void yAxisPlotMarkersChanged();
    void globalFontUpdate();
    void useTopDefaultChanged(bool);
    void useBottomDefaultChanged(bool);
    void useLeftDefaultChanged(bool);
    void useRightDefaultChanged(bool);
    void useAxisDefaultChanged(bool);
  private:
    void setupContent();
    void setupAxis();
    void setupLabels();
    void setupMarkers();
    void addRelations();
    void updateRelations();

  private:
    PlotItem* _plotItem;

    ContentTab *_contentTab;
    AxisTab *_xAxisTab;
    AxisTab *_yAxisTab;
    MarkersTab *_xMarkersTab;
    MarkersTab *_yMarkersTab;
    LabelTab *_labelTab;
    OverrideLabelTab *_topLabelTab;
    OverrideLabelTab *_bottomLabelTab;
    OverrideLabelTab *_leftLabelTab;
    OverrideLabelTab *_rightLabelTab;
    OverrideLabelTab *_axisLabelTab;
    PlotMarkersTab* _plotMarkersTab;
    DialogPageTab *_labelPage;


    QList<DialogPage*> _relationPages;

    ObjectStore* _store;
};

}

#endif

// vim: ts=2 sw=2 et
