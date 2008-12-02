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

#include "datasourceselectordialog.h"

#include "datasource.h"

#include <QMessageBox>
#include <QDebug>

namespace Kst {

DataSourceSelectorDialog::DataSourceSelectorDialog(QString &file, QWidget *parent)
  : QFileDialog(parent) {

  setFileMode(QFileDialog::Directory);
  selectFile(file);
  currentChanged(file);

  connect(this, SIGNAL(currentChanged(const QString &)), this, SLOT(currentChanged(const QString &)));
}


DataSourceSelectorDialog::~DataSourceSelectorDialog() {
}


QString DataSourceSelectorDialog::selectedDataSource() {
  return selectedFiles().first();
}


void DataSourceSelectorDialog::currentChanged(const QString &current) {
//   qDebug() << "currentChanged" << current;
  if (current.isEmpty()) {
    setFileMode(QFileDialog::Directory);
  } else {
    QFileInfo fileInfo(current);
    if (fileInfo.isDir()) {
//       qDebug() << "Directory Selected - valid?" << DataSource::validSource(current);
      if (DataSource::validSource(current)) {
        setFileMode(QFileDialog::Directory);
      } else {
        setFileMode(QFileDialog::ExistingFile);
      }
    } else if (fileInfo.exists()) {
//       qDebug() << "File Selected - valid?" << DataSource::validSource(current);
      if (DataSource::validSource(current)) {
        setFileMode(QFileDialog::ExistingFile);
      } else {
        setFileMode(QFileDialog::Directory);
      }
    }
  }
  QStringList filters;
  filters << "Any files (*)";
  setNameFilters(filters);
}


void DataSourceSelectorDialog::accept() {
  QStringList files = selectedFiles();
  if (files.isEmpty()) {
    return;
  }

  for (int i = 0; i < files.count(); ++i) {
      QFileInfo info(files.at(i));
      if (!info.exists()) {
#ifndef QT_NO_MESSAGEBOX
  QString message = tr("%1\nFile not found.\nPlease verify the "
                      "correct file name was given.");
  QMessageBox::warning(this, windowTitle(), message.arg(info.fileName()));
#endif // QT_NO_MESSAGEBOX
          return;
      }
      if (info.isDir()) {
          if (info.fileName().isEmpty() || (info.filePath() == directory().path())) {
            QDialog::accept();
            return;
          }
      }
  }

  QFileDialog::accept();
}

}

// vim: ts=2 sw=2 et
