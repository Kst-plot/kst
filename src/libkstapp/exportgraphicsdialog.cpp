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

#include "exportgraphicsdialog.h"

#include "mainwindow.h"
#include <QDir>
#include <QPictureIO>
#include <QDebug>
#include <QImageWriter>
#include <QTimer>
#include <QFileInfo>
#include <QPushButton>

namespace Kst {

ExportGraphicsDialog::ExportGraphicsDialog(MainWindow *parent)
  : QDialog(parent) {
  setupUi(this);

  _saveLocation->setFile(QDir::currentPath());

  _autoSaveTimer = new QTimer(this);

  QStringList formats;// = QPictureIO::outputFormats();
  foreach(QByteArray array, QImageWriter::supportedImageFormats()) {
    formats.append(QString(array));
  }

  _comboBoxFormats->addItems(formats);
  _comboBoxFormats->setCurrentIndex(0);

  connect(_autoSaveTimer, SIGNAL(timeout()),      this, SLOT(createFile()));
  connect(_comboBoxSizeOption, SIGNAL(activated(int)), this, SLOT(enableWidthHeight()));

  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
}


ExportGraphicsDialog::~ExportGraphicsDialog() {
}


void ExportGraphicsDialog::enableWidthHeight() {
  int displayOption = _comboBoxSizeOption->currentIndex();

  switch (displayOption) {
    case 0:
      _xSize->setEnabled(true);
      _ySize->setEnabled(true);
      break;
    case 1:
      _xSize->setEnabled(true);
      _ySize->setEnabled(false);
      break;
    case 2:
      _xSize->setEnabled(true);
      _ySize->setEnabled(false);
      break;
    case 3:
      _xSize->setEnabled(false);
      _ySize->setEnabled(true);
      break;
  }
}


void ExportGraphicsDialog::applyAutosave() {
  if (_autosave->isChecked()) {
    _autoSaveTimer->start(_period->value()*1000);
  } else {
    _autoSaveTimer->stop();
  }
}


void ExportGraphicsDialog::apply() {
  applyAutosave();

  if (!_autosave->isChecked()) {
    createFile();
  }
}


void ExportGraphicsDialog::createFile() {
  emit exportGraphics(_saveLocation->file(), _comboBoxFormats->currentText(), _xSize->value(), _ySize->value(), _comboBoxSizeOption->currentIndex());
}


void ExportGraphicsDialog::updateButtons() {
  bool valid = QFileInfo(_saveLocation->file()).isFile();
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
}


void ExportGraphicsDialog::OKClicked() {
  apply();
  accept();
}

}

// vim: ts=2 sw=2 et
