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

#include "editmultiplewidget.h"

#include <QRegExp>
#include <QDebug>

namespace Kst {

EditMultipleWidget::EditMultipleWidget(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_selectAllBut, SIGNAL(clicked()), this, SLOT(selectAllObjects()));

  connect(_selectNoneBut, SIGNAL(clicked()), _objectList, SLOT(clearSelection()));

  connect(_filterEdit, SIGNAL(textChanged(const QString&)), this, SLOT(applyFilter(const QString&)));
}


EditMultipleWidget::~EditMultipleWidget() {}


void EditMultipleWidget::selectAllObjects() {
  _objectList->selectAll();
}


void EditMultipleWidget::applyFilter(const QString& filter) {
  _objectList->clearSelection();

  // case insensitive and wildcards
  QRegExp re(filter, Qt::CaseInsensitive, QRegExp::Wildcard);

  uint c = _objectList->count();
  for (uint i = 0; i < c; ++i) {
    if (re.exactMatch(_objectList->item(i)->text())) {
      _objectList->item(i)->setSelected(true);
    }
  }
}


// void EditMultipleWidget::addObjects(QStringList &objects) {
//   _objectList->clear();
//   _objectList->addItems(objects);
// }


void EditMultipleWidget::clearObjects() {
  _objectList->clear();
}

void EditMultipleWidget::addObject(QString name, QString descriptionTip) {
  QListWidgetItem *wi = new QListWidgetItem(name);
  wi->setToolTip(descriptionTip);
  _objectList->addItem(wi);
}

QStringList EditMultipleWidget::selectedObjects() {
  QStringList objects;
  QList<QListWidgetItem *> selectedItems = _objectList->selectedItems();
  foreach (QListWidgetItem *item, selectedItems) {
    objects.append(item->text());
  }
  return objects;
}

}

// vim: ts=2 sw=2 et
