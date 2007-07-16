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
#include "plotrenderer2d.h"

namespace Kst {

class PlotItem : public ViewItem
{
  Q_OBJECT
  public:
    PlotItem(View *parent);
    virtual ~PlotItem();
    void paint(QPainter *painter);

    // List of renderers to use
    QList<PlotRenderer2D> renderers;

  private:

    // Options common to the plot and all rendered data
    QColor _backgroundColor;
    QStack<QColor> _colorStack;

};

class KST_EXPORT CreatePlotCommand : public CreateCommand
{
  public:
    CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
    CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
    virtual ~CreatePlotCommand() {}
    virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
