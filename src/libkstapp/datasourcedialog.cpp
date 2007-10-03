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

#include "datasourcedialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

namespace Kst {

DataSourceDialog::DataSourceDialog(DataDialog::EditMode mode, DataSourcePtr dataSource, QWidget *parent)
  : QDialog(parent), _dataSource(dataSource) {

  setWindowTitle(QString("Configure %1").arg(_dataSource->fileType()));

  QVBoxLayout *layout = new QVBoxLayout(this);

  _dataSource->readLock();
  QWidget *widget = _dataSource->configWidget();
  connect(this, SIGNAL(ok()), widget, SLOT(save()));

  if (mode == DataDialog::Edit) {
    connect(this, SIGNAL(ok()), this, SLOT(disableReuse()));
  }

  _dataSource->unlock();

  widget->setParent(this);
  layout->addWidget(widget);

  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(_buttonBox);

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton *)),
          this, SLOT(buttonClicked(QAbstractButton *)));

  setLayout(layout);

  setMaximumSize(QSize(1024, 768));
  resize(minimumSizeHint());
}


DataSourceDialog::~DataSourceDialog() {
}


void DataSourceDialog::disableReuse() {
  _dataSource->disableReuse();
}


void DataSourceDialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Ok:
    emit ok();
    accept();
    break;
  case QDialogButtonBox::Cancel:
    emit cancel();
    reject();
    break;
  default:
    break;
  }
}

}

// vim: ts=2 sw=2 et
