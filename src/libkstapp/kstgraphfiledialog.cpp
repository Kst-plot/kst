/***************************************************************************
                     kstgraphdialog.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtimer.h>

// include files for KDE
#include <kconfig.h>
#include <kimageio.h>
#include <kurlcompletion.h>
#include <kurlrequester.h>

// application specific includes
#include "kstgraphfiledialog.h"


KstGraphFileDialogI::KstGraphFileDialogI(QWidget* parent, const char* name,
                                         bool modal, Qt::WFlags fl)
: KstGraphFileDialog(parent, name, modal, fl) {
  _autoSaveTimer = new QTimer(this);

  connect(_autoSaveTimer, SIGNAL(timeout()),      this, SLOT(reqGraphFile()));
  connect(_ok,            SIGNAL(clicked()),      this, SLOT(ok_I()));
  connect(_Apply,         SIGNAL(clicked()),      this, SLOT(apply_I()));
  connect(_comboBoxSizeOption, SIGNAL(activated(int)), this, SLOT(enableWidthHeight()));
  connect(_comboBoxFormats, SIGNAL(activated(const QString&)), this, SLOT(enableEPSVector(const QString&)));
  
  _saveLocation->setFilter(KImageIO::mimeTypes().join(" "));
  _saveLocation->setMode(KFile::File);

  _comboBoxFormats->insertStrList(QImageIO::outputFormats());
  _comboBoxFormats->setCurrentItem(0);

  loadProperties();

  applyAutosave();
}


KstGraphFileDialogI::~KstGraphFileDialogI() {
}


void  KstGraphFileDialogI::show_I() {
  loadProperties();
  updateDialog();
  show();
  raise();
}


void KstGraphFileDialogI::ok_I() {
  apply_I();
  hide();
}


void KstGraphFileDialogI::apply_I() {
  _url = _saveLocation->url();
  _format = _comboBoxFormats->currentText();
  _w = _xSize->value();
  _h = _ySize->value();
  _displayOption = _comboBoxSizeOption->currentItem();
  _allWindows = _radioButtonAll->isChecked();
  _autoSave = _autosave->isChecked();
  _savePeriod = _period->value();
  _saveEPSAsVector = _saveEPSVector->isChecked();
  applyAutosave();
  
  if (!_autoSave) {
    if (_format == "EPS" && _saveEPSAsVector) {
      reqEpsGraphFile();
    } else {
      reqGraphFile();
    }
  }
  saveProperties();
}


void KstGraphFileDialogI::reqGraphFile() {
  emit graphFileReq(_url, _format, _w, _h, _allWindows, _displayOption);
}


void KstGraphFileDialogI::reqEpsGraphFile() {
  emit graphFileEpsReq(_url, _w, _h, _allWindows, _displayOption);
}


void KstGraphFileDialogI::applyAutosave() {
  if (_autoSave) {
    _autoSaveTimer->start(_savePeriod*1000, false);
  } else {
    _autoSaveTimer->stop();
  }
}


void KstGraphFileDialogI::setURL(const QString& url) {
  QString path;

  if (url.isEmpty()) {
    path = QDir::currentDirPath();
  } else {
    path = url;
  }

  _url = path;
}


void KstGraphFileDialogI::saveProperties() {
  KConfig cfg("kstrc", false, false);

  cfg.setGroup("AutoSaveImages");

  //cfg.writeEntry("AutoSave", _autosave); // not read
  cfg.writeEntry("Seconds", _savePeriod);
  cfg.writeEntry("Location", _url);

  cfg.writeEntry("XSize", _w);
  cfg.writeEntry("YSize", _h);
  cfg.writeEntry("Display", _displayOption);
  cfg.writeEntry("Square", _displayOption == 1);
  cfg.writeEntry("All", _allWindows);
  cfg.writeEntry("Format", _format);
  cfg.writeEntry("EPSVector", _saveEPSAsVector);
  
  cfg.sync();
}


void KstGraphFileDialogI::loadProperties() {
  KConfig cfg("kstrc");
  bool isSquare;

  cfg.setGroup("AutoSaveImages");

  _url = cfg.readEntry("Location", "");
  if (_url.isEmpty()) {
    _url = QDir::currentDirPath();
  }
  _format = cfg.readEntry("Format", "PNG");
  _w = cfg.readNumEntry("XSize", 640);
  _h = cfg.readNumEntry("YSize", 480);
  isSquare = cfg.readBoolEntry("Square", false);
  if (isSquare) {
    _displayOption = 1;
  } else {
    _displayOption = cfg.readNumEntry("Display", 0);
  }
  _allWindows = cfg.readBoolEntry("All", false);
  _autoSave = false; // do not read from config file...
  _savePeriod = cfg.readNumEntry("Seconds", 15);
  _saveEPSAsVector = cfg.readBoolEntry("EPSVector", true);
}


void KstGraphFileDialogI::enableEPSVector(const QString &format) {
  // FIXME: i18n
  _saveEPSVector->setEnabled(format == "EPS");
}


void KstGraphFileDialogI::enableWidthHeight() {
  int displayOption = _comboBoxSizeOption->currentItem();

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


void KstGraphFileDialogI::updateDialog() {
  if (_url.isEmpty()) {
    _url = QDir::currentDirPath();
  }
  _saveLocation->setURL(_url);
  _saveLocation->completionObject()->setDir(_url);

  QString upfmt = _format.upper();
  for (int i = 0; i < _comboBoxFormats->count(); i++) {
    if (_comboBoxFormats->text(i).upper() == upfmt) {
      _comboBoxFormats->setCurrentItem(i);
      break;
    }
  }

  _xSize->setValue(_w);
  _ySize->setValue(_h);
  _comboBoxSizeOption->setCurrentItem(_displayOption);
  _radioButtonAll->setChecked(_allWindows);
  _radioButtonActive->setChecked(!_allWindows);
  _autosave->setChecked(_autoSave);
  _saveOnce->setChecked(!_autoSave);
  _period->setValue(_savePeriod);
  _period->setEnabled(_autoSave);
  _saveEPSVector->setChecked(_saveEPSAsVector);
  
  enableEPSVector(_comboBoxFormats->currentText());
  enableWidthHeight();
}

#include "kstgraphfiledialog.moc"
// vim: ts=2 sw=2 et
