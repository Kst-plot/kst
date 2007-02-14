/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "kststring.h"

void DataSourceMetaDataDialog::init()
{
}


void DataSourceMetaDataDialog::setDataSource(KstDataSourcePtr dsp)
{
    _dsp = dsp;
    _name->clear();
    _source->clear();
    _plugin->clear();
    _value->clear();
    if (_dsp) {
      dsp->readLock();
      for (QDictIterator<KstString> i(dsp->metaData()); i.current(); ++i) {
          _name->insertItem(i.currentKey());
      }
      _source->setText(dsp->fileName());
      _plugin->setText(dsp->fileType());
      if (_dsp->hasMetaData(_name->currentText())) {
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


void DataSourceMetaDataDialog::updateMetadata(const QString& tag)
{
    _dsp->readLock();
    _value->setText(_dsp->metaData()[tag]->value());
    _dsp->unlock();
}


// vim: ts=8 sw=4 noet
