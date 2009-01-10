//
// C++ Interface: rangetab
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RANGETAB_H
#define RANGETAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_rangetab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT RangeTab : public DialogTab, Ui::RangeTab {
  Q_OBJECT
  public:
    RangeTab(PlotItem* plotItem, QWidget *parent = 0);
    virtual ~RangeTab();

    void setupRange();

    bool xAuto() {return _xAuto->isChecked();}
    bool xSpike() {return _xSpike->isChecked();}
    bool xBorder() {return _xBorder->isChecked();}
    bool xMean() {return _xMean->isChecked();}
    double xRange() {return _xRange->text().toDouble();}
    bool xFixed() { return _xFixed->isChecked();}
    double xMin() {return _xMin->text().toDouble();}
    double xMax() {return _xMax->text().toDouble();}

    bool yAuto() {return _yAuto->isChecked();}
    bool ySpike() {return _ySpike->isChecked();}
    bool yBorder() {return _yBorder->isChecked();}
    bool yMean() {return _yMean->isChecked();}
    double yRange() {return _yRange->text().toDouble();}
    bool yFixed() { return _yFixed->isChecked();}
    double yMin() {return _yMin->text().toDouble();}
    double yMax() {return _yMax->text().toDouble();}

  private:
    PlotItem *_plotItem;

  private Q_SLOTS:
    void modified();

  Q_SIGNALS:
    void tabModified();
};

}


#endif

// vim: ts=2 sw=2 et
