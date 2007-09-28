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

#ifndef EQUATIONDIALOG_H
#define EQUATIONDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "kst_export.h"

#include "ui_equationtab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT EquationTab : public DataTab, Ui::EquationTab {
  Q_OBJECT
  public:
    EquationTab(QWidget *parent = 0);
    virtual ~EquationTab();

  private Q_SLOTS:
  private:
};

class KST_EXPORT EquationDialog : public DataDialog {
  Q_OBJECT
  public:
    EquationDialog(QWidget *parent = 0);
    EquationDialog(KstObjectPtr dataObject, QWidget *parent = 0);
    virtual ~EquationDialog();

  protected:
    virtual QString tagName() const;
    virtual KstObjectPtr createNewDataObject() const;
    virtual KstObjectPtr editExistingDataObject() const;

  private:
    EquationTab *_equationTab;
};

}


#endif
// vim: ts=2 sw=2 et
