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

#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "histogram.h"

#include "ui_histogramtab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT HistogramTab : public DataTab, Ui::HistogramTab {
  Q_OBJECT
  public:
    HistogramTab(QWidget *parent = 0);
    virtual ~HistogramTab();

    VectorPtr vector() const;
    CurveAppearance* curveAppearance() const;
    CurvePlacement* curvePlacement() const;

    double min() const;
    double max() const;
    int bins() const;
    Histogram::NormalizationType normalizationType() const;

  private Q_SLOTS:
    void generateAutoBin();
    void updateButtons();
  private:
};

class KST_EXPORT HistogramDialog : public DataDialog {
  Q_OBJECT
  public:
    HistogramDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~HistogramDialog();

  protected:
    virtual QString tagName() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private:
    HistogramTab *_histogramTab;
};

}

#endif

// vim: ts=2 sw=2 et
