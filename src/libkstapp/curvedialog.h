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

#ifndef CURVEDIALOG_H
#define CURVEDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "ui_curvetab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT CurveTab : public DataTab, Ui::CurveTab {
  Q_OBJECT
  public:
    CurveTab(QWidget *parent = 0);
    virtual ~CurveTab();

  private Q_SLOTS:
  private:
};

class KST_EXPORT CurveDialog : public DataDialog {
  Q_OBJECT
  public:
    CurveDialog(KstObjectPtr dataObject, QWidget *parent = 0);
    virtual ~CurveDialog();

  protected:
    virtual QString tagName() const;
    virtual KstObjectPtr createNewDataObject() const;
    virtual KstObjectPtr editExistingDataObject() const;

  private:
    CurveTab *_curveTab;
};

}

#endif

// vim: ts=2 sw=2 et
