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

#include "scalardialog.h"
#include "enodes.h"

namespace Kst {

ScalarTab::ScalarTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Scalar"));
}


ScalarTab::~ScalarTab() {
}


DataSourcePtr ScalarTab::dataSource() const {
  return _dataSource;
}


void ScalarTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


QString ScalarTab::value() const {
  return _scalarValue->text();
}


void ScalarTab::setValue(const QString &value) {
  _scalarValue->setText(value);
}


ScalarDialog::ScalarDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Scalar"));
  else
    setWindowTitle(tr("New Scalar"));

  _scalarTab = new ScalarTab(this);
  addDataTab(_scalarTab);

}


ScalarDialog::~ScalarDialog() {}


QString ScalarDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr ScalarDialog::createNewDataObject() const {
  const DataSourcePtr dataSource = _scalarTab->dataSource();

  bool ok = false;
  QString tagName = DataDialog::tagName();
  double value = _scalarTab->value().toFloat(&ok);
  if (!ok) {
    value = Equations::interpret(_scalarTab->value().toLatin1(), &ok);
  }

//   qDebug() << "Creating new scalar  ===>"
//            << "\n\ttag:" << DataDialog::tagName()
//            << "\n\tvalue:" << value
//            << endl;

    if (ok) {
      ScalarPtr scalar = new Scalar(ObjectTag(DataDialog::tagName(), ObjectTag::globalTagContext), 0L, value);

      scalar->setOrphan(true);
      scalar->setEditable(true);

      scalar->writeLock();
      scalar->update(0);
      scalar->unlock();

      return static_cast<ObjectPtr>(scalar);
  }

  return 0;
}


ObjectPtr ScalarDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}
// vim: ts=2 sw=2 et
