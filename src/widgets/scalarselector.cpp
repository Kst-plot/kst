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

#include "scalarselector.h"

#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"

#include "enodes.h"

namespace Kst {

ScalarSelector::ScalarSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _store(store) {

  _defaultsSet = false;
  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newScalar->setIcon(QPixmap(":kst_scalarnew.png"));
  _editScalar->setIcon(QPixmap(":kst_scalaredit.png"));

  _newScalar->setFixedSize(size + 8, size + 8);
  _editScalar->setFixedSize(size + 8, size + 8);
  _selectScalar->setFixedSize(size + 8, size + 8);

  _scalarListSelector = new ScalarListSelector(this);

  connect(_newScalar, SIGNAL(pressed()), this, SLOT(newScalar()));
  connect(_editScalar, SIGNAL(pressed()), this, SLOT(editScalar()));
  connect(_selectScalar, SIGNAL(pressed()), this, SLOT(selectScalar()));
  connect(_scalar, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSelectionChanged()));
  connect(_scalar, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDescriptionTip()));
}


ScalarSelector::~ScalarSelector() {
}


void ScalarSelector::setObjectStore(ObjectStore *store) {
  _store = store;
  fillScalars();
}


void ScalarSelector::updateDescriptionTip() {
  setToolTip(selectedScalarString());
}


void ScalarSelector::emitSelectionChanged() {
  emit selectionChanged(_scalar->currentText());
}


void ScalarSelector::setDefaultValue(double value) {
 QString string = QString::number(value);
 int index = _scalar->findText(string);
 if (index<0) {
   _scalar->addItem(string, qVariantFromValue(0));
   _scalar->setCurrentIndex(_scalar->findText(string));
 } else {
   _scalar->setCurrentIndex(index);
 }
 _defaultsSet = true;
}


ScalarPtr ScalarSelector::selectedScalar() {
  bool existingScalar;
  if (_scalar->findText(_scalar->currentText(), Qt::MatchExactly) == -1) {
    // Value typed in.
    existingScalar = false;
  } else {
    if (qVariantValue<Scalar*>(_scalar->itemData(_scalar->findText(_scalar->currentText())))) {
      existingScalar = true;
    } else {
      // Default Value.  Doesn't exist as scalar yet.
      existingScalar = false;
    }
  }

  if (!existingScalar) {
    // Create the Scalar.
    bool ok = false;
    double value = _scalar->currentText().toDouble(&ok);
    if (!ok) {
      value = Equations::interpret(_store, _scalar->currentText().toLatin1(), &ok);
    }

    if (!ok) {
      return 0;
    }

    // Check if a scalar with this value exist & is orphan.
    foreach(Scalar* scalar, _store->getObjects<Scalar>()) {
      if (scalar->orphan()) {
        if (scalar->value() == value) {
          return scalar;
        }
      }
    }

    ScalarPtr scalar = _store->createObject<Scalar>();
    scalar->setValue(value);
    scalar->setEditable(true);
    scalar->setDescriptiveName(QString());
    scalar->setOrphan(false);

    _scalar->clearEditText();
    fillScalars();
    scalar->setOrphan(true);

    scalar->writeLock();
    scalar->registerChange();
    scalar->unlock();

    setSelectedScalar(scalar);

    return scalar;
  }
  return qVariantValue<Scalar*>(_scalar->itemData(_scalar->currentIndex()));
}


QString ScalarSelector::selectedScalarString() {
  if (_scalar->findText(_scalar->currentText(),Qt::MatchExactly) == -1) {
    return _scalar->currentText();
  }

  Scalar* scalar = qVariantValue<Scalar*>(_scalar->itemData(_scalar->currentIndex()));
  if (scalar) {
    return scalar->descriptionTip();
  } else {
    return QString();
  }
}


void ScalarSelector::setSelectedScalar(ScalarPtr selectedScalar) {
  int i=-1,j;
  for (j=0; j<_scalar->count() ; j++) {
    if (selectedScalar.data() == (qVariantValue<Scalar*>(_scalar->itemData(j)))) {
      i=j;
      break;
    }
  }

  if (i==-1) {
    setDefaultValue(selectedScalar->value());
  } else {
    _scalar->setCurrentIndex(i);
    _defaultsSet = false;
  }
}

void ScalarSelector::newScalar() {
  QString scalarName;
  DialogLauncher::self()->showScalarDialog(scalarName, 0, true);
  fillScalars();
  ScalarPtr scalar = kst_cast<Scalar>(_store->retrieveObject(scalarName));

  if (scalar) {
    setSelectedScalar(scalar);
    emitSelectionChanged();
  }
}


void ScalarSelector::editScalar() {
  if (selectedScalar()->provider()) {
    DialogLauncher::self()->showObjectDialog(selectedScalar()->provider());
  } else {
    QString scalarName;
    DialogLauncher::self()->showScalarDialog(scalarName, ObjectPtr(selectedScalar()), true);
  }
  fillScalars();
}


void ScalarSelector::selectScalar() {
  if (_scalarListSelector->exec() == QDialog::Accepted) {
    _scalar->setCurrentIndex(_scalar->findText(_scalarListSelector->selectedScalar()));
  }
}


void ScalarSelector::fillScalars() {
  if (!_store) {
    return;
  }

  QHash<QString, ScalarPtr> scalars;

  ScalarList scalarList = _store->getObjects<Scalar>();

  ScalarList::ConstIterator it = scalarList.begin();
  for (; it != scalarList.end(); ++it) {
    ScalarPtr scalar = (*it);

    scalar->readLock();
    if (!scalar->orphan()) {
      scalars.insert(scalar->sizeLimitedName(_scalar), scalar);
    }
    scalar->unlock();
  }

  QStringList list = scalars.keys();

  qSort(list);

  QString current_text = _scalar->currentText();
  ScalarPtr current = qVariantValue<Scalar*>(_scalar->itemData(_scalar->currentIndex()));;

  _scalar->clear();
  foreach (const QString &string, list) {
    ScalarPtr v = scalars.value(string);
    _scalar->addItem(string, qVariantFromValue(v.data()));
  }

  _scalarListSelector->clear();
  _scalarListSelector->fillScalars(list);

  if (current) {
    setSelectedScalar(current);
  } else {
    _scalar->addItem(current_text, qVariantFromValue(0));
    _scalar->setCurrentIndex(_scalar->findText(current_text));
    _defaultsSet = true;
  }

  _editScalar->setEnabled(_scalar->count() > 0);
  _selectScalar->setEnabled(_scalar->count() > 0);
}


bool ScalarSelector::event(QEvent * event) {
  if ((event->type() == QEvent::Resize) || (event->type() == QEvent::WindowActivate)) {
    if (_defaultsSet) {
      QString defaultText = _scalar->currentText();
      fillScalars();
      setDefaultValue(defaultText.toDouble());
    } else {
      fillScalars();
    }
  }
  return QWidget::event(event);
}
}

// vim: ts=2 sw=2 et
