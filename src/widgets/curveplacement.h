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

#ifndef CURVEPLACEMENT_H
#define CURVEPLACEMENT_H

#include <QWidget>
#include "ui_curveplacement.h"

#include "kst_export.h"

namespace Kst {

class PlotItemInterface;

class KST_EXPORT CurvePlacement : public QWidget, public Ui::CurvePlacement {
  Q_OBJECT
  public:
    enum Place { NewPlot, ExistingPlot, NoPlot };
    CurvePlacement(QWidget *parent = 0);
    virtual ~CurvePlacement();

    Place place() const;
    void setPlace(Place place);

    PlotItemInterface *existingPlot() const;
    void setExistingPlots(const QList<PlotItemInterface*> &existingPlots);

    bool createLayout() const;
    void setCreateLayout(bool createLayout);

    bool appendToLayout() const;
    void setAppendToLayout(bool appendToLayout);
};

}

#endif

// vim: ts=2 sw=2 et
