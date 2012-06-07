/**************************r*************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BADDATASOURCEDIALOG_H
#define BADDATASOURCEDIALOG_H

#include <QDialog>

#include "ui_baddatasourcedialog.h"
#include "datasource.h"

#include "kst_export.h"

namespace Kst {

class BadDatasourceDialog : public QDialog, Ui::BadDatasourceDialog
{
  Q_OBJECT
  public:
    BadDatasourceDialog(QString *filename, ObjectStore *store, QWidget *parent=NULL);
    virtual ~BadDatasourceDialog();

  private Q_SLOTS:
    void fileNameChanged(const QString &file);
    void sourceValid(QString filename, int requestID);
    void showConfigWidget();
    void skip();
    void change();
  private:
    ObjectStore *_store;
    DataSourcePtr _dataSource;
    int _requestID;
    QString *_fileName;
};
}

#endif

// vim: ts=2 sw=2 et
