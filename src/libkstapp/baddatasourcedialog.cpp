/***************************************************************************
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

#include <config.h>
#include "baddatasourcedialog.h"
#include "datasourcepluginmanager.h"
#include "datasourcedialog.h"


#include <QDebug>
#include <QThreadPool>


namespace Kst {

BadDatasourceDialog::BadDatasourceDialog(QString *filename, ObjectStore *store, QWidget *parent)
  : QDialog(parent), _store(store), _dataSource(0), _requestID(0), _fileName(filename)  {
   setupUi(this);

   connect(_url, SIGNAL(changed(const QString&)), this, SLOT(fileNameChanged(const QString&)));
   connect(_configureSource, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
   connect(_skip, SIGNAL(clicked()), this, SLOT(skip()));
   connect(_change, SIGNAL(clicked()), this, SLOT(change()));

   _label->setText(*filename + tr(": File not found.  "));
   _url->setFile(*filename);
   filename->clear();
   QApplication::restoreOverrideCursor();
}


BadDatasourceDialog::~BadDatasourceDialog() {
}

void BadDatasourceDialog::showConfigWidget() {
  QPointer<DataSourceDialog> dialog = new DataSourceDialog(DataDialog::New, _dataSource, this);
  if ( dialog->exec() == QDialog::Accepted ) {
    fileNameChanged(_dataSource->fileName());
  }
  delete dialog;
}


void BadDatasourceDialog::fileNameChanged(const QString &file) {
  _dataSource = 0;
  _configureSource->setEnabled(false);
  _change->setEnabled(false);
  _fileType->setText(QString());

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}

void BadDatasourceDialog::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);
  _fileType->setText(_dataSource->fileType());
  _configureSource->setEnabled(_dataSource->hasConfigWidget());
  _change->setEnabled(true);

}

void BadDatasourceDialog::skip() {
  _fileName->clear();
  close();
}

void BadDatasourceDialog::change() {
  *_fileName = _url->file();
  close();
}

}
// vim: ts=2 sw=2 et
