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

#ifndef PLOTITEMMANAGER_H
#define PLOTITEMMANAGER_H

#include <QObject>

#include <QHash>
#include <QList>

namespace Kst {

class View;
class ViewItem;
class PlotItem;
class SharedAxisBoxItem;

class PlotItemManager : public QObject
{
  Q_OBJECT
  public:
    static QList<PlotItem*> plotsForView(View *view);
    static QList<PlotItem*> tiedZoomPlotsForView(View *view);
    static QList<PlotItem*> tiedZoomPlotsForViewItem(ViewItem *viewItem);
    static QList<PlotItem*> tiedZoomPlots(PlotItem* plotItem);
    static QList<ViewItem*> tiedZoomViewItems(PlotItem* plotItem);
    static PlotItemManager *self();

    void setAllTiedZoom(View *view, bool tiedZoom);

  Q_SIGNALS:
    void tiedZoomRemoved();
    void allPlotsTiedZoom();

  public Q_SLOTS:
    void clearFocusedPlots();

  private:
    static void cleanup();

    PlotItemManager();
    virtual ~PlotItemManager();

    void addPlot(PlotItem *plotItem);
    void addViewItem(ViewItem *viewItem);
    void removePlot(PlotItem *plotItem);
    void removeViewItem(ViewItem *viewItem);

    void addTiedZoomPlot(PlotItem *plotItem, bool checkAll = true);
    void addTiedZoomViewItem(ViewItem *viewItem, bool checkAll = true);
    void removeTiedZoomPlot(PlotItem *plotItem);
    void removeTiedZoomViewItem(ViewItem *viewItem);

    void setFocusPlot(PlotItem *plotItem);
    void removeFocusPlot(PlotItem *plotItem);

    void checkAllTied(View* view);

  private:
    friend class ViewItem;
    friend class PlotItem;
    friend class SharedAxisBoxItem;
    QHash< View*, QList<PlotItem*> > _plotLists;
    QHash< View*, QList<ViewItem*> > _viewItemLists;
    QHash< View*, QList<PlotItem*> > _tiedZoomViewPlotLists;
    QHash< View*, QList<ViewItem*> > _tiedZoomViewItemLists;
    QHash< ViewItem*, QList<PlotItem*> > _tiedZoomViewItemPlotLists;
    QList<PlotItem*> _focusedPlots;
};

}

#endif

// vim: ts=2 sw=2 et
