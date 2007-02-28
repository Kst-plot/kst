/**************************************************************************
              kstquickstartdialog.cpp - quickstart dialog: inherits designer dialog
                             -------------------
    begin                :  2004
    copyright            : (C) 2004 University of British Columbia
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
#include <q3listbox.h>

// include files for KDE
#include <kurlcompletion.h>
#include <kurlrequester.h>

// application specific includes
#include "kst.h"
#include "kstdoc.h"
#include "kstquickstartdialog.h"
#include "kstsettings.h"

KstQuickStartDialogI::KstQuickStartDialogI(QWidget *parent, const char *name, bool modal, Qt::WFlags fl)
: KstQuickStartDialog(parent, name, modal, fl) {
  _fileName->completionObject()->setDir(QDir::currentDirPath());
  _app = KstApp::inst();
  _isRecentFile = false;
  connect(_startDataWizard, SIGNAL(clicked()), this, SLOT(wizard_I()));
  connect(_openFile, SIGNAL(clicked()), this, SLOT(open_I()));
  connect(_recentFileList, SIGNAL(highlighted(const QString&)), this, SLOT(changeURL(const QString&)));
  connect(_showAtStartup, SIGNAL(clicked()), this, SLOT(updateSettings()));
  connect(_fileName, SIGNAL(textChanged(const QString&)), this, SLOT(deselectRecentFile()));
  connect(_recentFileList, SIGNAL(selected(const QString&)), this, SLOT(open_I()));
}


KstQuickStartDialogI::~KstQuickStartDialogI() {
}


void KstQuickStartDialogI::wizard_I() {
  close();
  _app->showDataWizard();
}


void KstQuickStartDialogI::open_I() {
  if (_isRecentFile) {
    if (_app->slotFileOpenRecent(_fileName->url())) {
      // select the recently opened file...
      _app->selectRecentFile(_fileName->url());
      close();
    }
  } else if (_app->openDocumentFile(_fileName->url())) {
    close();
  }
}


void KstQuickStartDialogI::update() {
  //get the list of recent files
  _recentFileList->clear();
  _recentFileList->insertStringList(_app->recentFiles());

  //by default, select a recent file
  if (_recentFileList->numItemsVisible() > 0) {
    _recentFileList->setSelected(0, true);
  }

  //update the startup checkbox
  _showAtStartup->setChecked(KstSettings::globalSettings()->showQuickStart);
}


void KstQuickStartDialogI::show_I() {
  update();
  show();
  raise();
}


void KstQuickStartDialogI::changeURL(const QString& name) {
  _fileName->blockSignals(true);
  _fileName->setURL(name);
  _fileName->blockSignals(false);
  _isRecentFile = true;
}


void KstQuickStartDialogI::updateSettings() {
  KstSettings::globalSettings()->showQuickStart = _showAtStartup->isChecked();
  KstSettings::globalSettings()->save();
  emit settingsChanged();
}


void KstQuickStartDialogI::deselectRecentFile() {
  _recentFileList->clearSelection();
  _isRecentFile = false;
}

#include "kstquickstartdialog.moc"
// vim: et ts=2 sw=2
