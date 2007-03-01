/***************************************************************************
                   datasourcemetadialog.cpp
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

#include "datasourcemetadatadialog.h"

#include "kststring.h"

#include <kst_export.h>

DataSourceMetaDataDialog::DataSourceMetaDataDialog(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

//  connect(, SIGNAL(), this, SLOT());
}


DataSourceMetaDataDialog::~DataSourceMetaDataDialog() {}


void DataSourceMetaDataDialog::init() {}


void DataSourceMetaDataDialog::setDataSource(KstDataSourcePtr dsp) {
  _dsp = dsp;
  _name->clear();
  _source->clear();
  _plugin->clear();
  _value->clear();
  if (_dsp) {
    dsp->readLock();
    for (Q3DictIterator<KstString> i(dsp->metaData())
         ;
         i.current();
         ++i) {
      _name->insertItem(i.currentKey());
    }
    _source->setText(dsp->fileName());
    _plugin->setText(dsp->fileType());
    if (_dsp->hasMetaData(_name->currentText())
       ) {
      _value->setText(_dsp->metaData(_name->currentText()));
    }
    dsp->unlock();
    _name->setEnabled(_name->count() > 0);
    _value->setEnabled(_name->count() > 0);
  } else {
    _name->setEnabled(false);
    _value->setEnabled(false);
  }
}


void DataSourceMetaDataDialog::updateMetadata(const QString& tag) {
  _dsp->readLock();
  _value->setText(_dsp->metaData()[tag]->value());
  _dsp->unlock();
}

#include "datasourcemetadatadialog.moc"

// vim: ts=2 sw=2 et
