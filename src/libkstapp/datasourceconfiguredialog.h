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

#ifndef DATASOURCECONFIGUREDIALOG_H
#define DATASOURCECONFIGUREDIALOG_H

#include <QDialog>

#include "kstcore_export.h"

#include "datadialog.h"
#include "datasource.h"

class QAbstractButton;
class QDialogButtonBox;

namespace Kst {

class DataSourceConfigureDialog : public QDialog
{
  Q_OBJECT
  public:
    friend class DialogSI;
    DataSourceConfigureDialog(DataDialog::EditMode mode, DataSourcePtr dataSource, QWidget *parent = 0);
    virtual ~DataSourceConfigureDialog();

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
