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

#ifndef POWERSPECTRUMDIALOG_H
#define POWERSPECTRUMDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "psd.h"

#include "ui_powerspectrumtab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT PowerSpectrumTab : public DataTab, Ui::PowerSpectrumTab {
  Q_OBJECT
  public:
    PowerSpectrumTab(QWidget *parent = 0);
    virtual ~PowerSpectrumTab();

    VectorPtr vector() const;
    CurveAppearance* curveAppearance() const;
    CurvePlacement* curvePlacement() const;
    FFTOptions* FFTOptionsWidget() const;


  private Q_SLOTS:
  private:
};

class KST_EXPORT PowerSpectrumDialog : public DataDialog {
  Q_OBJECT
  public:
    PowerSpectrumDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~PowerSpectrumDialog();

  protected:
    virtual QString tagName() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private:
    PowerSpectrumTab *_powerSpectrumTab;
};

}

#endif
