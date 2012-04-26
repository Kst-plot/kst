/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATASOURCEDIALOG_H
#define DATASOURCEDIALOG_H

#include <QDialog>

#include "kst_export.h"

#include "datadialog.h"
#include "datasource.h"

class QAbstractButton;
class QDialogButtonBox;

namespace Kst {

class DataSourceDialog : public QDialog
{
  Q_OBJECT
  public:
    friend class DialogSI;
    DataSourceDialog(DataDialog::EditMode mode, DataSourcePtr dataSource, QWidget *parent = 0);
    virtual ~DataSourceDialog();

    DataSourcePtr dataSource() const { return _dataSource; }

  Q_SIGNALS:
    void ok();
    void cancel();

  private Q_SLOTS:
    void disableReuse();
    void buttonClicked(QAbstractButton *button);

  private:
    DataSourcePtr _dataSource;
    QDialogButtonBox *_buttonBox;
    DataSourceConfigWidget* _configWidget;
};

}

#endif

// vim: ts=2 sw=2 et
