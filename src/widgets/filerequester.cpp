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

#include <QStyle>
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDirModel>
#include <QCompleter>

#include <QDebug>

namespace Kst {

FileRequester::FileRequester(QWidget *parent)
  : QWidget(parent) {
  setup();
}


FileRequester::FileRequester(const QString &file, QWidget *parent)
  : QWidget(parent), _file(file) {
  setup();
}


FileRequester::~FileRequester() {
}


void FileRequester::setup() {

  _fileEdit = new QLineEdit(this);
  _fileButton = new QToolButton(this);

  QHBoxLayout * layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(_fileEdit);
  layout->addWidget(_fileButton);
  setLayout(layout);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);
  _fileButton->setIcon(QPixmap(":kst_changefile.png"));
  _fileButton->setFixedSize(size + 8, size + 8);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (_fileEdit, SIGNAL(textEdited(const QString &)), this, SLOT(setFile(const QString &)));
  connect (_fileButton, SIGNAL(clicked()), this, SLOT(chooseFile()));

  QDirModel *dirModel = new QDirModel(this);
  dirModel->setFilter(QDir::AllEntries);

  QCompleter *completer = new QCompleter(this);
  completer->setModel(dirModel); 

  _fileEdit->setCompleter(completer);
}


QString FileRequester::file() const {
  return _file;
}


void FileRequester::setFile(const QString &file) {
  _file = file;
  //FIXME grrr QLineEdit doc *lies* to me... the textEdited signal is being triggered!!
  _fileEdit->blockSignals(true);
  _fileEdit->setText(_file);
  _fileEdit->blockSignals(false);
  emit changed(file);
}


void FileRequester::chooseFile() {

  QString file = QFileDialog::getOpenFileName(this, QString(), _file);
  if (!file.isEmpty()) {
    setFile(file);
  }
}

}

// vim: ts=2 sw=2 et
