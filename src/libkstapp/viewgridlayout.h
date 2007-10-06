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

#ifndef VIEWGRIDLAYOUT_H
#define VIEWGRIDLAYOUT_H

#include <QObject>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QTransform>
#include <QHash>

namespace Kst {

class ViewItem;
class PlotItem;

class ViewGridLayout : public QObject
{
  Q_OBJECT
  public:
    ViewGridLayout(ViewItem *parent);
    virtual ~ViewGridLayout();

    ViewItem *parentItem() const;

    void addViewItem(ViewItem *viewItem, int row, int column);
    void addViewItem(ViewItem *viewItem, int row, int column, int rowSpan, int columnSpan);

    int rowCount() const;
    int columnCount() const;

    QSizeF spacing() const { return _spacing; }
    void setSpacing(const QSizeF &spacing) { _spacing = spacing; }

    QSizeF margin() const { return _margin; }
    void setMargin(const QSizeF &margin) { _margin = margin; }

    qreal plotMarginWidth(const PlotItem *plotItem) const;
    qreal plotMarginHeight(const PlotItem *plotItem) const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

  public Q_SLOTS:
    void reset();
    void resetSharedAxis();
    void update();

  Q_SIGNALS:
    void enabledChanged(bool enabled);

  private Q_SLOTS:
    void updatePlotMargins();

  private:
    struct LayoutItem {
      ViewItem *viewItem;
      int row;
      int column;
      int rowSpan;
      int columnSpan;
      QTransform transform;
      QPointF position;
      QRectF rect;
    };

  private:
    void updateSharedAxis();
    void shareAxisWithPlotToLeft(LayoutItem item);
    void shareAxisWithPlotToRight(LayoutItem item);
    void shareAxisWithPlotAbove(LayoutItem item);
    void shareAxisWithPlotBelow(LayoutItem item);

  private:
    bool _enabled;
    int _rowCount;
    int _columnCount;

    QSizeF _spacing;
    QSizeF _margin;

    QList<LayoutItem> _items;
    QHash<int, qreal> _plotMarginWidth;
    QHash<int, qreal> _plotMarginHeight;
    QHash<const ViewItem*, LayoutItem> _itemInfos;
    QHash< QPair<int, int>, LayoutItem> _itemLayouts;
};

//FIXME How far should we go with the command pattern?
//Should we provide undo support for layout margins?
//How about for setting the pens/brushes of a viewitem??
#if 0
class KST_EXPORT LayoutMarginCommand : public ViewItemCommand
{
  public:
    LayoutMarginCommand(ViewItem *item, const QPointF &originalMargin, const QPointF &newMargin)
        : ViewItemCommand(item, QObject::tr("Margin"))
          _originalMargin(originalMargin),
          _newMargin(newMargin),
          _layout(item->layout) {}

    virtual ~LayoutMarginCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QSizeF _originalMargin;
    QSizeF _newMargin;
    QPointer<ViewGridLayout> _layout;
};

class KST_EXPORT LayoutSpacingCommand : public ViewItemCommand
{
  public:
    LayoutSpacingCommand(ViewItem *item, const QPointF &originalSpacing, const QPointF &newSpacing)
        : ViewItemCommand(item, QObject::tr("Spacing"))
          _originalSpacing(originalSpacing),
          _newSpacing(newSpacing),
          _layout(item->layout) {}

    virtual ~LayoutSpacingCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QSizeF _originalSpacing;
    QSizeF _newSpacing;
    QPointer<ViewGridLayout> _layout;
};
#endif

}

#endif

// vim: ts=2 sw=2 et
