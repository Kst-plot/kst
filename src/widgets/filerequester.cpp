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

#include "filerequester.h"

#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>

namespace Kst {

FileRequester::FileRequester(QWidget *parent)
  : QWidget(parent) {

  _fileEdit = new QLineEdit(this);
  _fileButton = new QToolButton(this);

  QHBoxLayout * layout = new QHBoxLayout(this);
  layout->addWidget(_fileEdit);
  layout->addWidget(_fileButton);
  setLayout(layout);

  _fileButton->setIcon(QPixmap(":kst_changefile.png"));

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (_fileEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setFile(const QString &)));
  connect (_fileButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
}


FileRequester::FileRequester(const QString &file, QWidget *parent)
  : QWidget(parent), _file(file) {

  _fileEdit = new QLineEdit(this);
  _fileButton = new QToolButton(this);

  QHBoxLayout * layout = new QHBoxLayout(this);
  layout->addWidget(_fileEdit);
  layout->addWidget(_fileButton);
  setLayout(layout);

  _fileButton->setIcon(QPixmap(":kst_changefile.png"));

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (_fileEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setFile(const QString &)));
  connect (_fileButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
}


FileRequester::~FileRequester() {
}


QString FileRequester::file() const {
  return _file;
}


void FileRequester::setFile(const QString &file) {
  _file = file;
  emit changed(file);
}


void FileRequester::chooseFile() {

  bool ok;
  QString file = QFileDialog::getOpenFileName(this);
  if (ok && !file.isEmpty()) {
    _fileEdit->setText(file); //will call setFile...
  }
}

}

// vim: ts=2 sw=2 et
