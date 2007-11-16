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

#ifndef CSDDIALOG_H
#define CSDDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "csd.h"

#include "ui_csdtab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT CSDTab : public DataTab, Ui::CSDTab {
  Q_OBJECT
  public:
    CSDTab(QWidget *parent = 0);
    virtual ~CSDTab();

    void setObjectStore(ObjectStore *store);

    VectorPtr vector() const;
    void setVector(const VectorPtr vector);

    int windowSize() const;
    void setWindowSize(const int windowSize);

    CurvePlacement* curvePlacement() const;
    FFTOptions* FFTOptionsWidget() const;
    ColorPalette* colorPalette() const;

    void hideImageOptions();

  private Q_SLOTS:
    void selectionChanged();

  Q_SIGNALS:
    void optionsChanged();
};

class KST_EXPORT CSDDialog : public DataDialog {
  Q_OBJECT
  public:
    CSDDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~CSDDialog();

  protected:
    virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private Q_SLOTS:
    void updateButtons();

  private:
    void configureTab(ObjectPtr object);

    CSDTab *_CSDTab;
};

}

#endif
