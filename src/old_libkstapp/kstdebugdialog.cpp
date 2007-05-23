/**************************************************************************
              kstdebugdialog.cpp - debug dialog: inherits designer dialog
                             -------------------
    begin                :  2004
    copyright            : (C) 2004 The University of British Columbia
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
#include <qlabel.h>
#include <qlayout.h>
#include <q3listview.h>

// include files for KDE
#include <kcolorbutton.h>
#include <klocale.h>

#include "kst.h"
#include "kstdatacollection.h"
#include "kstdebugdialog.h"
#include "kstdoc.h"
#include "kstlogwidget.h"

/*
 *  Constructs a KstDebugDialogI which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KstDebugDialogI::KstDebugDialogI(QWidget* parent, Qt::WindowFlags fl)
: QDialog(parent, fl ) {

  setupUi(this);

  _log = new KstLogWidget(TabPage, "logwidget");
  _log->setDebug(KstDebug::self());

  gridLayout2->addMultiCellWidget(_log, 0, 0, 0, 2);

  const QStringList& pl = KstDataSource::pluginList();

  for (QStringList::ConstIterator it = pl.begin(); it != pl.end(); ++it) {
    new Q3ListViewItem(_dataSources, *it);
  }

  _buildInfo->setText(i18n("<h1>Kst</h1> version %1 (%2)").arg(KSTVERSION).arg(KstDebug::self()->kstRevision()));

  connect(KstApp::inst()->document(), SIGNAL(logAdded(const KstDebug::LogMessage&)), _log, SLOT(logAdded(const KstDebug::LogMessage&)));
  connect(KstApp::inst()->document(), SIGNAL(logCleared()), _log, SLOT(clear()));
  connect(_email, SIGNAL(clicked()), this, SLOT(email()));
  connect(_clear, SIGNAL(clicked()), this, SLOT(clear()));
  connect(checkBoxShowDebug, SIGNAL(toggled(bool)), _log, SLOT(setShowDebug(bool)));
  connect(checkBoxShowWarning, SIGNAL(toggled(bool)), _log, SLOT(setShowWarning(bool)));
  //connect(checkBoxShowOther, SIGNAL(toggled(bool)), _log, SLOT(messagesOther()));
  checkBoxShowOther->hide(); // unused
  connect(checkBoxShowNotice, SIGNAL(toggled(bool)), _log, SLOT(setShowNotice(bool)));
  connect(checkBoxShowError, SIGNAL(toggled(bool)), _log, SLOT(setShowError(bool)));

  connect(_close, SIGNAL(clicked()), this, SLOT(close()));
}


KstDebugDialogI::~KstDebugDialogI() {
}


void KstDebugDialogI::init() {
  _dataSources->setAllColumnsShowFocus(true);
}


void KstDebugDialogI::show_I() {
  // Inefficient but shouldn't be a factor
  Q3ListViewItemIterator it(_dataSources);
  KST::dataSourceList.lock().readLock();
  while (it.current()) {
    while (it.current()->childCount() > 0) {
      delete it.current()->firstChild();
    }

    for (KstDataSourceList::Iterator i = KST::dataSourceList.begin(); i != KST::dataSourceList.end(); ++i) {
      (*i)->readLock();
      if ((*i)->sourceName() == it.current()->text(0)) {
        new Q3ListViewItem(it.current(), QString::null, (*i)->fileName());
      }
      (*i)->unlock();
    }
    ++it;
  }
  KST::dataSourceList.lock().unlock();

  QDialog::show();
}


void KstDebugDialogI::clear() {
  KstDebug::self()->clear();
  _log->clear();
}


void KstDebugDialogI::email() {
  KstDebug::self()->sendEmail();
}


KstLogWidget *KstDebugDialogI::logWidget() const {
  return _log;
}


#include "kstdebugdialog.moc"
// vim: et ts=2 sw=2
