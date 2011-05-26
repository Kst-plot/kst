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

#ifndef PLOTITEMDIALOG_H
#define PLOTITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class ContentTab;
class PlotMarkersTab;
class AxisTab;
class RangeTab;
class MarkersTab;
class LabelTab;
class ObjectStore;
class PlotItem;
class OverrideLabelTab;
class DialogPageTab;
class PlotAxis;
class PlotMarkers;

class PlotItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    explicit PlotItemDialog(PlotItem *item, QWidget *parent = 0);
    virtual ~PlotItemDialog();

  private Q_SLOTS:
    void contentChanged();
    //void relationChanged();
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
    void rangeChanged();
    void editMultiple();
    void editSingle();
    void slotApply();

  private:
    void setupContent();
    void setupAxis();
    void setupRange();
    void setupLabels();
    void setupMarkers();
    //void addRelations();
    //void updateRelations();

    void saveAxis(PlotAxis *item, AxisTab *axisTab);
    void saveRange(PlotItem *item);
    void saveLabels(PlotItem *item);
    void saveMarkers(PlotAxis *item, PlotMarkers& markers);

  private:
    PlotItem* _plotItem;
    QString _defaultTagString;

    ContentTab *_contentTab;
    AxisTab *_xAxisTab;
    AxisTab *_yAxisTab;
    RangeTab *_rangeTab;
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


    //QList<DialogPage*> _relationPages;

    ObjectStore* _store;
};

}

#endif

// vim: ts=2 sw=2 et
