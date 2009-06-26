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

#ifndef CHANGEDATASAMPLEDIALOG_H
#define CHANGEDATASAMPLEDIALOG_H

#include <QDialog>

#include "ui_changedatasampledialog.h"

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT ChangeDataSampleDialog : public QDialog, Ui::ChangeDataSampleDialog
{
  Q_OBJECT
  public:
    ChangeDataSampleDialog(QWidget *parent);
    virtual ~ChangeDataSampleDialog();

    void show();

  private slots:
    void addButtonClicked();
    void removeButtonClicked();
    void addAll();
    void removeAll();

    void availableDoubleClicked(QListWidgetItem * item);
    void selectedDoubleClicked(QListWidgetItem * item);

    void modified();
    void updateButtons();

    void initializeEntries();
    void updateDefaults(QListWidgetItem* item);

    void OKClicked();
    void apply();


  private:
    void updateCurveListDialog();

    ObjectStore *_store;

};

}

#endif

// vim: ts=2 sw=2 et
