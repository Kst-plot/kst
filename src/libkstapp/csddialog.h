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

    VectorPtr vector() const;
    CurvePlacement* curvePlacement() const;
    FFTOptions* FFTOptionsWidget() const;


  private Q_SLOTS:
  private:
};

class KST_EXPORT CSDDialog : public DataDialog {
  Q_OBJECT
  public:
    CSDDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~CSDDialog();

  protected:
    virtual QString tagName() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private:
    CSDTab *_CSDTab;
};

}

#endif
