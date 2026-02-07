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

#include "datasourceselector.h"

#include "datasourceselectordialog.h"
#include "geticon.h"

#include <QStyle>
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QCompleter>
#include <QPointer>

#include <QDebug>
#include <QLabel>

namespace Kst {

DataSourceSelector::DataSourceSelector(QWidget *parent)
  : QWidget(parent), _mode(QFileDialog::ExistingFile) {
  setup();
}


DataSourceSelector::~DataSourceSelector() {
}


void DataSourceSelector::setup() {

  _fileEdit = new QLineEdit(this);
  _fileButton = new QToolButton(this);

  int h = fontMetrics().lineSpacing()*4/3;

  _fileEdit->setFixedHeight(h);


  QHBoxLayout * layout = new QHBoxLayout(this);
  layout->setContentsMargins({});
  layout->addWidget(_fileEdit);
  layout->addWidget(_fileButton);

  _fileButton->setFixedSize(h,h);
  setLayout(layout);

  //int size = style()->pixelMetric(QStyle::PM_SmallIconSize);
  _fileButton->setFixedSize(h,h);
  _fileButton->setIcon(KstGetIcon("kst_changefile"));
  //qDebug() << "file button small icon size" << size;

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  //connect (_fileEdit, SIGNAL(textChanged(const QString &)), this, SIGNAL(changed(const QString &)));
  connect (_fileEdit, SIGNAL(textChanged(QString)), this, SLOT(updateFile(QString)));
  connect (_fileButton, SIGNAL(clicked()), this, SLOT(chooseFile()));

  QFileSystemModel *dirModel = new QFileSystemModel;
  dirModel->setFilter(QDir::AllEntries);
  dirModel->setRootPath(QString('/'));

  QCompleter *completer = new QCompleter(this);
  completer->setModel(dirModel); 

  _fileEdit->setCompleter(completer);
  setFixedHeight(h);
}


QString DataSourceSelector::file() const {
  return QDir::cleanPath(_file);
}


void DataSourceSelector::setFile(const QString &file) {
  _file = file;
  //FIXME grrr QLineEdit doc *lies* to me... the textEdited signal is being triggered!!
  _fileEdit->blockSignals(true);
  _fileEdit->setText(_file);
  _fileEdit->blockSignals(false);
  emit changed(file);
}

void DataSourceSelector::updateFile(const QString &file) {
  if (file.contains('~')) {
    QString home = qgetenv("HOME"); // linux
    if (!home.isEmpty()) {
      QString changed_file = file;
      changed_file.replace('~', home);
      setFile(changed_file);
    }
    home = qgetenv("USERPROFILE"); // windows, maybe (?)
    if (!home.isEmpty()) {
      QString changed_file = file;
      changed_file.replace('~', home);
      setFile(changed_file);
    }
  } else {
    _file = file;
    emit changed(file);
  }
}



void DataSourceSelector::chooseFile() {
  QString file;
  QPointer<DataSourceSelectorDialog> dialog = new DataSourceSelectorDialog( _file, this );

  dialog->setFileMode(QFileDialog::ExistingFile);

  if (dialog->exec() == QDialog::Accepted) {
    file = dialog->selectedDataSource();

    if (!file.isEmpty()) {
      setFile(file);
    }
  }

  delete dialog;
}

}

// vim: ts=2 sw=2 et
