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

#include "changefiledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "datamatrix.h"

#include <QDir>

namespace Kst {

ChangeFileDialog::ChangeFileDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  // TODO Need Icon.
  _clearFilter->setText("Clear Filter");

  connect(_changeFileCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(_changeFileOK, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_changeFileApply, SIGNAL(clicked()), this, SLOT(accept()));

  connect(_changeFileClear, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_changeFileClear, SIGNAL(clicked()), _changeFileCurveList, SLOT(clearSelection()));
  connect(_changeFileSelectAll, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_changeFileSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
  connect(_clearFilter, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_clearFilter, SIGNAL(clicked()), _changeFileCurveList, SLOT(clearSelection()));

  connect(_duplicateSelected, SIGNAL(toggled(bool)), _duplicateDependents, SLOT(setEnabled(bool)));

  connect(_filter, SIGNAL(textChanged(const QString&)), this, SLOT(updateSelection(const QString&)));


  _dataFile->setFile(QDir::currentPath());

}


ChangeFileDialog::~ChangeFileDialog() {
}


void ChangeFileDialog::exec() {
  updateCurveList();
  QDialog::exec();
}


void ChangeFileDialog::updateCurveList() {
  DataVectorList dataVectorList; //FIXME //= ObjectSubList<Vector, DataVector>(vectorList);
  DataMatrixList dataMatrixList; //FIXME //= ObjectSubList<Matrix, DataMatrix>(matrixList);
  QMap<QString, QString> filesUsed;
  int i;

  _changeFileCurveList->clear();

  for (i = 0; i < (int)dataVectorList.count(); i++) {
    dataVectorList[i]->readLock();
    _changeFileCurveList->addItem(dataVectorList[i]->tag().displayString());
    filesUsed.insert(dataVectorList[i]->filename(), dataVectorList[i]->filename());
    dataVectorList[i]->unlock();
  }

  for (i = 0; i < (int)dataMatrixList.count(); i++) {
    dataMatrixList[i]->readLock();
    _changeFileCurveList->addItem(dataMatrixList[i]->tag().displayString());
    filesUsed.insert(dataMatrixList[i]->filename(), dataMatrixList[i]->filename());
    dataMatrixList[i]->unlock();
  }

  QString currentFile = _files->currentText();
  _files->clear();

//  dataSourceList.lock();
  DataSourceList dataSourceList;  // FIXME
  for (DataSourceList::Iterator it = dataSourceList.begin(); it != dataSourceList.end(); ++it) {
    if (filesUsed.contains((*it)->fileName())) {
      _files->addItem((*it)->fileName());
    }
  }

  _allFromFile->setEnabled(_files->count() > 0);
  _files->setEnabled(_files->count() > 0);
}


void ChangeFileDialog::selectAll() {
  _changeFileCurveList->selectAll();
}


void ChangeFileDialog::updateSelection(const QString& txt) {
  _changeFileCurveList->clearSelection();
  QRegExp re(txt, Qt::CaseSensitive, QRegExp::Wildcard);
  for (int i = 0; i < _changeFileCurveList->count(); ++i) {
    _changeFileCurveList->item(i)->setSelected(re.exactMatch(_changeFileCurveList->item(i)->text()));
  }
}

}

// vim: ts=2 sw=2 et
