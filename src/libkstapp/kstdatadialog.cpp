/***************************************************************************
                   kstdatadialog.cpp
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
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

#include "kstdatadialog.h"

#include "kst.h"
#include "kstdoc.h"

#include <kst_export.h>
//Added by qt3to4:
#include <QCloseEvent>

#include <qvbox.h>
#include <qlistbox.>
#include "editmultiplewidget.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"

KstDataDialog::KstDataDialog(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

 connect(_ok, SIGNAL(clicked()), this, SLOT(ok()));

 connect(_cancel, SIGNAL(clicked()), this, SLOT(close()));
}


KstDataDialog::~KstDataDialog() {}


void KstDataDialog::ok() {
  _ok->setEnabled(false);
  _cancel->setEnabled(false);
  if (_newDialog || _dp == 0L) {
    if (newObject()) {
      close();
    } else {
      _ok->setEnabled(true);
      _cancel->setEnabled(true);
    }
  } else {
    if (editObject()) {
      close();
    } else {
      _ok->setEnabled(true);
      _cancel->setEnabled(true);
    }
  }
}


void KstDataDialog::close() {
  _dp = 0L;
  QDialog::close();
}


void KstDataDialog::reject() {
  _dp = 0L;
  QDialog::reject();
}


void KstDataDialog::init() {
  _dp = 0L;
  _newDialog = false;
  _multiple = false;
  _editMultipleMode = false;
  connect(this, SIGNAL(modified()), KstApp::inst()->document(), SLOT(wasModified()));
  connect(_editMultiple, SIGNAL(clicked()), this, SLOT(toggleEditMultiple()));
  _editMultiple->hide();
  _editMultipleWidget->hide();
}


void KstDataDialog::update() {}


void KstDataDialog::show() {
  showNew(QString::null);
}


void KstDataDialog::showNew(const QString& field) {
  Q_UNUSED(field) //used by plugin dialogs which inherit this class
  _newDialog = true;
  _dp = 0L;

  update();
  fillFieldsForNew();

  _editMultiple->hide();
  _editMultipleWidget->hide();
  _editMultipleMode = false;
  _tagName->setEnabled(true);
  _legendText->setEnabled(true);

  setCaption(i18n("New %1").arg(objectName()));
  QDialog::show();
  raise();
  _ok->setEnabled(true);
  _cancel->setEnabled(true);
}


void KstDataDialog::showEdit(const QString& field) {
  _newDialog = false;
  _dp = findObject(field);

  if (!_dp) {
    show();
    return;
  }

  if (_multiple) {
    _editMultiple->show();
    _editMultiple->setText(i18n("Edit Multiple >>"));
    _editMultipleWidget->hide();
    _editMultipleMode = false;
  }

  _tagName->setEnabled(true);
  _legendText->setEnabled(true);

  update();
  fillFieldsForEdit();

  setCaption(i18n("Edit %1").arg(objectName()));
  QDialog::show();
  raise();
  _ok->setEnabled(true);
  _cancel->setEnabled(true);
}


QString KstDataDialog::objectName() {
  return QString::null;
}


void KstDataDialog::fillFieldsForEdit() {}


void KstDataDialog::fillFieldsForNew() {}


KstObjectPtr KstDataDialog::findObject( const QString & name ) {
  KST::dataObjectList.lock().readLock();
  KstObjectPtr o = (*KST::dataObjectList.findTag(name)).data();
  KST::dataObjectList.lock().unlock();
  return o;
}


bool KstDataDialog::newObject() {
  return false;
}


bool KstDataDialog::editObject() {
  return false;
}


void KstDataDialog::populateEditMultiple() {}


bool KstDataDialog::multiple() {
  return _multiple;
}


void KstDataDialog::setMultiple(bool multiple) {
  _multiple = multiple;
}


void KstDataDialog::toggleEditMultiple() {
  if (_multiple) {
    if (_editMultipleMode) {
      cleanup();
      showEdit(_dp->tagName()); // redisplay the edit dialog
    } else {
      _editMultipleMode = true;
      _editMultipleWidget->_objectList->clear();
      // fill in list of objects and prepare the fields for multiple edit
      populateEditMultiple();
      _editMultipleWidget->show();
      _editMultiple->setText(i18n("Edit Multiple <<"));
      adjustSize();
      resize(minimumSizeHint());
      setFixedHeight(height());
    }
  }
}


void KstDataDialog::closeEvent(QCloseEvent *e) {
  cleanup();
  QWidget::closeEvent(e);
}


void KstDataDialog::cleanup() {}

#include "kstdatadialog.moc"

// vim: ts=2 sw=2 et
