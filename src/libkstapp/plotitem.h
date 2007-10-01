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

#ifndef PLOTITEM_H
#define PLOTITEM_H

#include <QObject>
#include <QColor>
#include <QStack>
#include <QGraphicsItem>

#include "viewitem.h"
#include "kstrelation.h"

namespace Kst {

class PlotRenderItem;

class PlotItem : public ViewItem
{
  Q_OBJECT
  public:
    PlotItem(View *parent);
    virtual ~PlotItem();

    /* Returns a list of the PlotItems in the main window's
     * current view.
     */
    static QList<PlotItem*> plotItems();

    QList<PlotRenderItem*> renderItems() const;
    void addRenderItem(PlotRenderItem *renderItem);
    void removeRenderItem(PlotRenderItem *renderItem);
    void clearRenderItems();

    virtual void paint(QPainter *painter);

    qreal marginWidth() const;
    qreal marginHeight() const;

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

  private:
    qreal calculatedMarginWidth() const;
    void setCalculatedMarginWidth(qreal marginWidth);

    qreal calculatedMarginHeight() const;
    void setCalculatedMarginHeight(qreal marginHeight);

    QRectF horizontalLabelRect(bool calc) const;
    QRectF verticalLabelRect(bool calc) const;

    void paintLeftLabel(QPainter *painter);
    QSizeF calculateLeftLabelBound(QPainter *painter);
    void paintBottomLabel(QPainter *painter);
    QSizeF calculateBottomLabelBound(QPainter *painter);
    void paintRightLabel(QPainter *painter);
    QSizeF calculateRightLabelBound(QPainter *painter);
    void paintTopLabel(QPainter *painter);
    QSizeF calculateTopLabelBound(QPainter *painter);

  private:
    QList<PlotRenderItem*> _renderers;
    qreal _calculatedMarginWidth;
    qreal _calculatedMarginHeight;

    friend class ViewGridLayout;
};

class KST_EXPORT CreatePlotCommand : public CreateCommand
{
  public:
    CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
    CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
    virtual ~CreatePlotCommand() {}
    virtual void createItem();
};

class KST_EXPORT CreatePlotForCurve : public CreateCommand
{
  public:
    CreatePlotForCurve() : CreateCommand(QObject::tr("Create Plot For Curve")) {}
    CreatePlotForCurve(View *view) : CreateCommand(view, QObject::tr("Create Plot For Curve")) {}
    virtual ~CreatePlotForCurve() {}
    virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
