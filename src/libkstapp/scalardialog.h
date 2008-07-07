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

#ifndef SCALARDIALOG_H
#define SCALARDIALOG_H

#include "datadialog.h"
#include "datatab.h"
#include "datasource.h"

#include "ui_scalartab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT ScalarTab : public DataTab, Ui::ScalarTab {
  Q_OBJECT
  public:
    ScalarTab(QWidget *parent = 0);
    virtual ~ScalarTab();

    QString value() const;
    void setValue(const QString &value);

  Q_SIGNALS:
    void valueChanged();

  private Q_SLOTS:
    void textChanged();
};

class KST_EXPORT ScalarDialog : public DataDialog {
  Q_OBJECT
  public:
    ScalarDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~ScalarDialog();

  protected:
//     virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr editExistingDataObject() const;

  private:
    void configureTab(ObjectPtr object);

  private Q_SLOTS:
    void updateButtons();

  private:
    ScalarTab *_scalarTab;
};

}

#endif

// vim: ts=2 sw=2 et
