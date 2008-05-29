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

    void setObjectStore(ObjectStore *store);

    VectorPtr vector() const;
    bool vectorDirty() const;
    void setVector(const VectorPtr vector);

    CurveAppearance* curveAppearance() const;
    CurvePlacement* curvePlacement() const;
    FFTOptions* FFTOptionsWidget() const;

    void hideCurveOptions();
    void clearTabValues();

  private Q_SLOTS:    
    void selectionChanged();

  Q_SIGNALS:
    void vectorChanged();
};

class KST_EXPORT PowerSpectrumDialog : public DataDialog {
  Q_OBJECT
  public:
    PowerSpectrumDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~PowerSpectrumDialog();

    void setVector(VectorPtr vector);

  protected:
//     virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private Q_SLOTS:
    void updateButtons();
    void editMultipleMode();
    void editSingleMode();

  private:
    void configureTab(ObjectPtr object=0);

    PowerSpectrumTab *_powerSpectrumTab;
};

}

#endif
