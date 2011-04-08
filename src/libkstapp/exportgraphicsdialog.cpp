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

#include "exportgraphicsdialog.h"
#include "dialogdefaults.h"

#include "mainwindow.h"
#include <QDir>
#include <QPictureIO>
#include <QDebug>
#include <QImageWriter>
#include <QTimer>
#include <QFileInfo>
#include <QPushButton>
#include <QLineEdit>

namespace Kst {

ExportGraphicsDialog::ExportGraphicsDialog(MainWindow *parent)
  : QDialog(parent) {
  setupUi(this);

  _saveLocation->setFile(_dialogDefaults->value("export/filename",QDir::currentPath()).toString());

  _autoSaveTimer = new QTimer(this);

  _listVectorFormats->setChecked(_dialogDefaults->value("export/useVectors",true).toBool());
  _listBitmapFormats->setChecked(_dialogDefaults->value("export/useBitmaps",true).toBool());

  updateFormats();

  _xSize->setValue(_dialogDefaults->value("export/xsize","1024").toInt());
  _ySize->setValue(_dialogDefaults->value("export/ysize","768").toInt());

  _comboBoxSizeOption->setCurrentIndex(_dialogDefaults->value("export/sizeOption","0").toInt());
  enableWidthHeight();

  _saveLocationLabel->setBuddy(_saveLocation->_fileEdit);

  connect(_autoSaveTimer, SIGNAL(timeout()),      this, SLOT(createFile()));
  connect(_comboBoxSizeOption, SIGNAL(activated(int)), this, SLOT(enableWidthHeight()));

  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(_listVectorFormats, SIGNAL(clicked()), this, SLOT(updateFormats()));
  connect(_listBitmapFormats, SIGNAL(clicked()), this, SLOT(updateFormats()));
}


ExportGraphicsDialog::~ExportGraphicsDialog() {
}

void ExportGraphicsDialog::updateFormats() {
  QStringList formats;// = QPictureIO::outputFormats();

  if (_listBitmapFormats->isChecked()) {
    foreach(QByteArray array, QImageWriter::supportedImageFormats()) {
      formats.append(QString(array));
    }
    formats.removeAll(QString("eps"));
    formats.removeAll(QString("EPS"));
  }

  if (_listVectorFormats->isChecked()) {
    formats.append(QString("svg"));
    formats.append(QString("eps"));
    formats.append(QString("pdf"));
  }


  _comboBoxFormats->clear();
  _comboBoxFormats->addItems(formats);

  int index = _comboBoxFormats->findText(_dialogDefaults->value("export/format","png").toString());
  if (index<0) {
    if (_listBitmapFormats->isChecked()) {
      index = _comboBoxFormats->findText("png");
    } else {
      index = _comboBoxFormats->findText("svg");
    }
  }
  _comboBoxFormats->setCurrentIndex(index);

}

void ExportGraphicsDialog::enableWidthHeight() {
  int displayOption = _comboBoxSizeOption->currentIndex();

  switch (displayOption) {
  case 0: // Width and Maintain Aspect Ratio
    _xSize->setEnabled(true);
    _ySize->setEnabled(false);
    _widthLabel->setEnabled(true);
    _heightLabel->setEnabled(false);
    break;
  case 1: // Height and Maintain Aspect Ratio
    _xSize->setEnabled(false);
    _ySize->setEnabled(true);
    _widthLabel->setEnabled(false);
    _heightLabel->setEnabled(true);
    break;
  case 2: // Width and Height
    _xSize->setEnabled(true);
    _ySize->setEnabled(true);
    _widthLabel->setEnabled(true);
    _heightLabel->setEnabled(true);
    break;
  case 3: // Size of Square
    _xSize->setEnabled(true);
    _ySize->setEnabled(false);
    _widthLabel->setEnabled(true);
    _heightLabel->setEnabled(false);
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
  QString filename = _saveLocation->file();
  QString format = _comboBoxFormats->currentText();
  if (_autoExtension->isChecked()) {
    if (QFileInfo(filename).suffix().toLower() != format.toLower()) {
      filename += '.' + format;
    }
  }
  _dialogDefaults->setValue("export/filename", filename);
  _dialogDefaults->setValue("export/format", format);
  _dialogDefaults->setValue("export/xsize", _xSize->value());
  _dialogDefaults->setValue("export/ysize", _ySize->value());
  _dialogDefaults->setValue("export/sizeOption", _comboBoxSizeOption->currentIndex());
  _dialogDefaults->setValue("export/useVectors", _listVectorFormats->isChecked());
  _dialogDefaults->setValue("export/useBitmaps", _listBitmapFormats->isChecked());
  emit exportGraphics(filename, format, _xSize->value(), _ySize->value(), _comboBoxSizeOption->currentIndex());
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
