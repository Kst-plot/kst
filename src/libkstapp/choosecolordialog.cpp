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

#include "choosecolordialog.h"

#include "datavector.h"
#include "datacollection.h"

#include "colorsequence.h"

namespace Kst {

ChooseColorDialog::ChooseColorDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

}


ChooseColorDialog::~ChooseColorDialog() {
}


void ChooseColorDialog::show() {
  updateColorGroup();
  QDialog::show();
}



void ChooseColorDialog::updateColorGroup() {

  // cannot use dataSourceList.fileNames() as it contains datasources that
  // are not used by any curves or vectors
  DataVectorList vcList = ObjectSubList<Vector, DataVector>(vectorList);

  // buildup a list of filenames
  QStringList fileNameList;
  for (DataVectorList::Iterator vc_iter = vcList.begin();
        vc_iter != vcList.end();
        ++vc_iter)
  {
    if (fileNameList.contains((*vc_iter)->filename()) == 0)
      fileNameList.push_back((*vc_iter)->filename());
  }

  // cleanup the dialog
  cleanColorGroup();

  // new grid
  grid = new QGridLayout(colorFrame, fileNameList.count(), 2, 0, 8);
  grid->setColumnStretch(1,0);

  int i = fileNameList.count();
  for (QStringList::Iterator it = fileNameList.begin();
        it != fileNameList.end();
        ++it)
  {
    QLineEdit* dataSourceName = new QLineEdit(colorFrame, "dataSourceName"+i);
    dataSourceName->setReadOnly(true);
    dataSourceName->setText(*it);
    grid->addWidget(dataSourceName,i,0);
    lineEdits.push_back(dataSourceName);
    dataSourceName->show();

    ColorButton* dataSourceColor = new ColorButton(colorFrame);
    dataSourceColor->setObjectName("dataSourceColor"+i);
    dataSourceColor->setColor(ColorSequence::next());
    grid->addWidget(dataSourceColor,i,1);
    colorButtons.push_back(dataSourceColor);
    dataSourceColor->show();
    i++;
  }

  adjustSize();
  resize(QSize(500, minimumSizeHint().height()));
  setFixedHeight(height());
}

void ChooseColorDialog::cleanColorGroup()
{
  while (!lineEdits.isEmpty())
  {
    QLineEdit* tempLineEdit = lineEdits.back();
    lineEdits.pop_back();
    delete tempLineEdit;
  }

  while (!colorButtons.isEmpty())
  {
    ColorButton* tempColorButton = colorButtons.back();
    colorButtons.pop_back();
    delete tempColorButton;
  }
  delete grid;
}

}

// vim: ts=2 sw=2 et
