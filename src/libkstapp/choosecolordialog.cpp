/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "choosecolordialog.h"

#include "datavector.h"
#include "datacollection.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"
#include "application.h"
#include "updatemanager.h"

#include "colorsequence.h"
#include <QPushButton>

namespace Kst {

ChooseColorDialog::ChooseColorDialog(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  MainWindow::setWidgetFlags(this);

  _grid = 0;

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
    // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChooseColorDialog without the object store");
  }
  QMap<DataSourcePtr, QColor> _dataSourceColors;
  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
}


ChooseColorDialog::~ChooseColorDialog() {
  delete _grid;
}


void ChooseColorDialog::show() {
  updateColorGroup();
  QDialog::show();
}


void ChooseColorDialog::updateColorGroup() {

  // cannot use dataSourceList.fileNames() as it contains datasources that
  // are not used by any curves or vectors
  DataVectorList vcList = _store->getObjects<DataVector>();

  _dataSourceColors.clear();
  for (DataVectorList::Iterator vc_iter = vcList.begin();
        vc_iter != vcList.end();
        ++vc_iter)
  {
    if (! (_dataSourceColors.contains((*vc_iter)->dataSource())))
      _dataSourceColors[(*vc_iter)->dataSource()] = (*vc_iter)->dataSource()->color();
  }

  cleanColorGroup();

  _grid = new QGridLayout(colorFrame);
  _grid->setSpacing(8);
  _grid->setColumnStretch(1,0);

  int i=0;
  QMapIterator<DataSourcePtr, QColor> it(_dataSourceColors);
  while (it.hasNext()) {
    it.next();
    QLineEdit* dataSourceName = new QLineEdit(colorFrame);
    dataSourceName->setReadOnly(true);
    dataSourceName->setText(it.key()->fileName());
    _grid->addWidget(dataSourceName,i,0);
    _lineEdits.push_back(dataSourceName);
    dataSourceName->show();

    ColorButton* dataSourceColor = new ColorButton(colorFrame);
    dataSourceColor->setColor(it.value());
    _grid->addWidget(dataSourceColor,i,1);
    _colorButtons.push_back(dataSourceColor);
    dataSourceColor->show();
    i++;
  }

  adjustSize();
  resize(QSize(500, minimumSizeHint().height()));
  setFixedHeight(height());
}


void ChooseColorDialog::cleanColorGroup() {
  while (!_lineEdits.isEmpty())
  {
    QLineEdit* tempLineEdit = _lineEdits.back();
    _lineEdits.pop_back();
    delete tempLineEdit;
  }

  while (!_colorButtons.isEmpty())
  {
    ColorButton* tempColorButton = _colorButtons.back();
    _colorButtons.pop_back();
    delete tempColorButton;
  }
  delete _grid;
}


void ChooseColorDialog::OKClicked() {
  if (_buttonBox->button(QDialogButtonBox::Apply)->isEnabled()) {
    apply();
  }
  accept();
}


void ChooseColorDialog::apply() {
  CurveList curveList = _store->getObjects<Curve>();
  for (CurveList::iterator curve_iter = curveList.begin(); curve_iter != curveList.end(); ++curve_iter)
  {
    VectorPtr vector;
    CurvePtr curve = kst_cast<Curve>(*curve_iter);
    if (_xVector->isChecked()) {
      vector = curve->xVector();
    } else {
      vector = curve->yVector();
    }
    if (DataVectorPtr dataVector = kst_cast<DataVector>(vector))
    {
      curve->writeLock();
      curve->setColor(getColorForFile(dataVector->filename()));
      curve->registerChange();
      curve->unlock();
    }
  }
  // Store the selected colors in the corresponding datasource objects
  QMutableMapIterator<DataSourcePtr, QColor> itDatasource(_dataSourceColors);
  QListIterator<ColorButton*> itColorButton(_colorButtons);
  DataSourcePtr ds;
  while (itDatasource.hasNext()) {
    ds = itDatasource.next().key();
    ds->setColor(itColorButton.next()->color()); // Per construction there should always be as many color buttons as datasources
  }

  updateColorGroup(); // This will update the _dataSourceColors map

  UpdateManager::self()->doUpdates(true);
  kstApp->mainWindow()->document()->setChanged(true);
}


QColor ChooseColorDialog::getColorForFile(const QString &fileName) {
  QList<ColorButton*>::Iterator kc_iter = _colorButtons.begin();
  for (QList<QLineEdit*>::Iterator fn_iter = _lineEdits.begin(); fn_iter != _lineEdits.end(); ++fn_iter) {
    if (fileName == (*fn_iter)->text()) {
      return (*kc_iter)->color();
    }
    ++kc_iter;
  }
  return QColor();   
}

}

// vim: ts=2 sw=2 et
