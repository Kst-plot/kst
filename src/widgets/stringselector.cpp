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

#include "stringselector.h"

#include "objectstore.h"

namespace Kst {

StringSelector::StringSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _store(store) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newString->setIcon(QPixmap(":kst_stringnew.png"));
  _editString->setIcon(QPixmap(":kst_stringedit.png"));

  _newString->setFixedSize(size + 8, size + 8);
  _editString->setFixedSize(size + 8, size + 8);
  _selectString->setFixedSize(size + 8, size + 8);
}


StringSelector::~StringSelector() {
}


void StringSelector::setObjectStore(ObjectStore *store) {
  _store = store;
}


StringPtr StringSelector::selectedString() const {
  return 0;
}


void StringSelector::setSelectedString(StringPtr selectedString) {
  Q_UNUSED(selectedString);
}

}

// vim: ts=2 sw=2 et
