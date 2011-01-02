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

#include "stringselector.h"

#include "dialoglauncher.h"
#include "objectstore.h"

namespace Kst {

StringSelector::StringSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _allowEmptySelection(false), _store(store) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newString->setIcon(QPixmap(":kst_stringnew.png"));
  _editString->setIcon(QPixmap(":kst_stringedit.png"));

  _newString->setFixedSize(size + 8, size + 8);
  _editString->setFixedSize(size + 8, size + 8);
  _selectString->setFixedSize(size + 8, size + 8);

  fillStrings();

  connect(_newString, SIGNAL(pressed()), this, SLOT(newString()));
  connect(_editString, SIGNAL(pressed()), this, SLOT(editString()));
  connect(_string, SIGNAL(activated(int)), this, SLOT(emitSelectionChanged()));
  connect(_string, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDescriptionTip()));
}


StringSelector::~StringSelector() {
}


void StringSelector::setObjectStore(ObjectStore *store) {
  _store = store;
  fillStrings();
}


void StringSelector::updateDescriptionTip() {
  if (selectedString()) {
    setToolTip(selectedString()->descriptionTip());
  } else {
    setToolTip(QString());
  }
}


void StringSelector::emitSelectionChanged() {
  emit selectionChanged(_string->currentText());
}


StringPtr StringSelector::selectedString() const {
  return qVariantValue<String*>(_string->itemData(_string->currentIndex()));
}


void StringSelector::setSelectedString(StringPtr selectedString) {
  int i=-1,j;
  for (j=0; j<_string->count() ; j++) {
    if (selectedString.data() == (qVariantValue<String*>(_string->itemData(j)))) {
      i=j;
      break;
    }
  }
  Q_ASSERT(i != -1);

  _string->setCurrentIndex(i);
}


void StringSelector::newString() {
  QString stringName;
  DialogLauncher::self()->showStringDialog(stringName, 0, true);
  fillStrings();
  StringPtr string = kst_cast<String>(_store->retrieveObject(stringName));

  if (string) {
    setSelectedString(string);
    emitSelectionChanged();
  }
}


void StringSelector::editString() {
  QString stringName;
  DialogLauncher::self()->showStringDialog(stringName, ObjectPtr(selectedString()), true);
  fillStrings();
}


void StringSelector::fillStrings() {
  if (!_store) {
    return;
  }

  QHash<QString, StringPtr> strings;

  StringList stringList = _store->getObjects<String>();

  StringList::ConstIterator it = stringList.begin();
  for (; it != stringList.end(); ++it) {
    StringPtr string = (*it);

    string->readLock();
    strings.insert(string->Name(), string);
    string->unlock();
  }

  QStringList list = strings.keys();

  qSort(list);

  StringPtr current = selectedString();

  _string->clear();
  foreach (const QString &string, list) {
    StringPtr s = strings.value(string);
    _string->addItem(string, qVariantFromValue(s.data()));
  }

  if (_allowEmptySelection) //reset the <None>
    setAllowEmptySelection(true);

  if (current)
    setSelectedString(current);

  _editString->setEnabled(_string->count() > 0);
}


bool StringSelector::allowEmptySelection() const {
  return _allowEmptySelection;
}


void StringSelector::setAllowEmptySelection(bool allowEmptySelection) {
  _allowEmptySelection = allowEmptySelection;

  int i = _string->findText(tr("<None>"));
  if (i != -1)
    _string->removeItem(i);

  if (_allowEmptySelection) {
    _string->insertItem(0, tr("<None>"), qVariantFromValue(0));
    _string->setCurrentIndex(0);
  }
}


bool StringSelector::event(QEvent * event) {
  if (event->type() == QEvent::WindowActivate) {
    fillStrings();
  }
  return QWidget::event(event);
}

}

// vim: ts=2 sw=2 et
