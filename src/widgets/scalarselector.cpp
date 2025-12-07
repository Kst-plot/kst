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
#include "updateserver.h"
#include "ui_scalarselector.h"

#include "enodes.h"
#include "geticon.h"

namespace Kst {

ScalarSelector::ScalarSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent)
  , _store(store)
  , ui(new Ui::ScalarSelector)
{
  _defaultsSet = false;
  ui->setupUi(this);
  if (property("isFOverSR").toBool()) {
    ui->_cutoffLabel->show();
    ui->_cutoff->show();
    ui->_SRLabel->show();
    ui->_SR->show();

  } else {
    ui->_cutoffLabel->hide();
    ui->_cutoff->hide();
    ui->_SRLabel->hide();
    ui->_SR->hide();
  }

  ui->_newScalar->setIcon(KstGetIcon("kst_scalarnew"));
  ui->_editScalar->setIcon(KstGetIcon("kst_scalaredit"));

  ui->_newScalar->setFixedSize(iconWidth(), iconWidth());
  ui->_editScalar->setFixedSize(iconWidth(), iconWidth());
  ui->_selectScalar->setFixedSize(iconWidth(), iconWidth());

  _scalarListSelector = new ScalarListSelector(this);


  //_scalar->resize(10,5);
  connect(ui->_newScalar, SIGNAL(pressed()), this, SLOT(newScalar()));
  connect(ui->_editScalar, SIGNAL(pressed()), this, SLOT(editScalar()));
  connect(ui->_selectScalar, SIGNAL(pressed()), this, SLOT(selectScalar()));
  connect(ui->_scalar, SIGNAL(editTextChanged(QString)), this, SLOT(emitSelectionChanged()));
  connect(ui->_scalar, SIGNAL(editTextChanged(QString)), this, SLOT(updateDescriptionTip()));
  connect(ui->_scalar, SIGNAL(editTextChanged(QString)), this, SLOT(ratioChanged()));
  connect(ui->_cutoff, SIGNAL(textEdited(QString)), this, SLOT(cutoffChanged()));
  connect(ui->_SR, SIGNAL(textChanged(QString)), this, SLOT(srChanged()));
  connect(ui->_SR, SIGNAL(textChanged(QString)), this, SIGNAL(SRChanged(QString)));

  connect(UpdateServer::self(), SIGNAL(objectListsChanged()), this, SLOT(updateScalarList()));

}


ScalarSelector::~ScalarSelector() {
}

int ScalarSelector::iconWidth() const {
  return fontMetrics().lineSpacing()*4/3;
}

void ScalarSelector::setIsFOverSR(bool is_f_over_sr)
{
  setProperty("isFOVerSR", is_f_over_sr);

  if (is_f_over_sr) {
    ui->_cutoffLabel->show();
    ui->_cutoff->show();
    ui->_SRLabel->show();
    ui->_SR->show();

    QSize size = ui->_scalar->size();
    size.setWidth(fontMetrics().horizontalAdvance("000000000"));
    ui->_SR->setMinimumSize(size);
    ui->_cutoff->setMinimumSize(size);

    size.setWidth(fontMetrics().horizontalAdvance("0000000000000"));
    ui->_scalar->setMinimumSize(size);

    //setMinimumWidth(3*min_width + _cutoffLabel->width() + _SRLabel->width()+3*_newScalar->width());

  } else {
    ui->_cutoffLabel->hide();
    ui->_cutoff->hide();
    ui->_SRLabel->hide();
    ui->_SR->hide();
  }

}

QSize ScalarSelector::minimumSizeHint() const {
  return QSize(15*fontMetrics().horizontalAdvance("m")+ 3 * iconWidth(), iconWidth());
}


void ScalarSelector::setObjectStore(ObjectStore *store) {
  _store = store;
  fillScalars();
}


void ScalarSelector::updateDescriptionTip() {
  bool editable;
  setToolTip(selectedScalarString(&editable));
  ui->_editScalar->setEnabled(editable);
}


void ScalarSelector::emitSelectionChanged() {
  emit selectionChanged(ui->_scalar->currentText());
}


void ScalarSelector::setDefaultValue(double value) {
 QString string = QString::number(value);
 int index = ui->_scalar->findText(string);
 if (index<0) {
   ui->_scalar->addItem(string, QVariant::fromValue(0));
   ui->_scalar->setCurrentIndex(ui->_scalar->findText(string));
 } else {
   ui->_scalar->setCurrentIndex(index);
 }
 _defaultsSet = true;
}


ScalarPtr ScalarSelector::selectedScalar() {
  bool existingScalar;
  if (ui->_scalar->findText(ui->_scalar->currentText(), Qt::MatchExactly) == -1) {
    // Value typed in.
    existingScalar = false;
  } else {
    if (ui->_scalar->itemData(ui->_scalar->findText(ui->_scalar->currentText())).value<Scalar*>()) {
      existingScalar = true;
    } else {
      // Default Value.  Doesn't exist as scalar yet.
      existingScalar = false;
    }
  }

  if (!existingScalar) {
    // Create the Scalar.
    bool ok = false;
    double value = ui->_scalar->currentText().toDouble(&ok);
    if (!ok) {
      value = Equations::interpret(_store, ui->_scalar->currentText().toLatin1(), &ok);
    }

    if (!ok) {
      return 0;
    }

    // Check if a scalar with this value exist & is hidden.
    foreach(Scalar* scalar, _store->getObjects<Scalar>()) {
      if (scalar->hidden()) {
        if (scalar->value() == value) {
          return scalar;
        }
      }
    }

    ScalarPtr scalar = _store->createObject<Scalar>();
    scalar->setValue(value);
    scalar->setEditable(false);
    scalar->setDescriptiveName(QString());
    scalar->setOrphan(true);
    scalar->setHidden(true);

    ui->_scalar->clearEditText();
    fillScalars();

    scalar->writeLock();
    scalar->registerChange();
    scalar->unlock();

    setSelectedScalar(scalar);

    return scalar;
  }
  return ui->_scalar->itemData(ui->_scalar->currentIndex()).value<Scalar*>();;
}


QString ScalarSelector::selectedScalarString(bool *editable) {
  if (ui->_scalar->findText(ui->_scalar->currentText(),Qt::MatchExactly) == -1) {
    if (editable) {
      *editable = false;
    }
    return ui->_scalar->currentText();
  }

  Scalar* scalar = ui->_scalar->itemData(ui->_scalar->currentIndex()).value<Scalar*>();;
  if (scalar) {
    if (editable) {
      *editable = scalar->editable();
    }
    return scalar->descriptionTip();
  } else {
    if (editable) {
      *editable = false;
    }
    return QString();
  }
}


void ScalarSelector::setSelectedScalar(QString Name) {
  bool ok;
  QString trimmed_name = Name.split('(').at(0).trimmed();
  double value = trimmed_name.toDouble(&ok);

  if (ok) {
    setDefaultValue(value);
  } else {
    Kst::Object* object = _store->retrieveObject(trimmed_name);
    Kst::Scalar* scalar = static_cast<Kst::Scalar*>(object);
    if (scalar) {
      setSelectedScalar(scalar);
    }
  }
}

void ScalarSelector::setSR(const QString &SR) {
  ui->_SR->setText(SR);
}

QString ScalarSelector::SR() {
  return ui->_SR->text();
}


void ScalarSelector::setSelectedScalar(ScalarPtr selectedScalar) {
  int i=-1;
  for (int j=0; j<ui->_scalar->count() ; ++j) {
    if (selectedScalar.data() == ui->_scalar->itemData(j).value<Scalar*>()) {
      i=j;
      break;
    }
  }

  if (i==-1) {
    setDefaultValue(selectedScalar->value());
  } else {
    ui->_scalar->setCurrentIndex(i);
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
    ui->_scalar->setCurrentIndex(ui->_scalar->findText(_scalarListSelector->selectedScalar()));
  }
}


void ScalarSelector::fillScalars() {
  if (!_store) {
    return;
  }

  QHash<QString, ScalarPtr> scalars;

  ScalarList scalarList = _store->getObjects<Scalar>();

  ScalarList::ConstIterator it = scalarList.constBegin();
  for (; it != scalarList.constEnd(); ++it) {
    ScalarPtr scalar = (*it);

    scalar->readLock();
    if (!scalar->hidden()) {
      scalars.insert(scalar->CleanedName(), scalar);
    }
    scalar->unlock();
  }

  QStringList list = scalars.keys();

  std::sort(list.begin(), list.end());

  QString current_text = ui->_scalar->currentText();
  ScalarPtr current = ui->_scalar->itemData(ui->_scalar->currentIndex()).value<Scalar*>();

  ui->_scalar->clear();
  foreach (const QString &string, list) {
    ScalarPtr v = scalars.value(string);
    ui->_scalar->addItem(string, QVariant::fromValue(v.data()));
  }

  _scalarListSelector->clear();
  _scalarListSelector->fillScalars(list);

  if (current) {
    setSelectedScalar(current);
  } else {
    ui->_scalar->addItem(current_text, QVariant::fromValue(0));
    ui->_scalar->setCurrentIndex(ui->_scalar->findText(current_text));
    _defaultsSet = true;
  }

  ui->_editScalar->setEnabled(ui->_scalar->count() > 0);
  ui->_selectScalar->setEnabled(ui->_scalar->count() > 0);
  updateDescriptionTip();
}

void ScalarSelector::updateScalarList() {
  if (_defaultsSet) {
    QString defaultText = ui->_scalar->currentText();
    fillScalars();
    setDefaultValue(defaultText.toDouble());
  } else {
    fillScalars();
  }
}

void ScalarSelector::ratioChanged() {
  updateFields(Ratio);
}

void ScalarSelector::cutoffChanged() {
  updateFields(Cutoff);
}

void ScalarSelector::srChanged() {
  updateFields(SampleRate);
}

void ScalarSelector::updateFields(ControlField control_field) {


  // Start Ratio
  // Last Cutoff
  // Range SR

  double ratio;
  double cutoff;
  double frequency;
  bool ok = false;

  // set ratio: the value of the selected scalar, or evaluate as equation, or convert to double.
  if (ui->_scalar->itemData(ui->_scalar->findText(ui->_scalar->currentText())).value<Scalar*>()) {
    ratio = ScalarPtr(ui->_scalar->itemData(ui->_scalar->findText(ui->_scalar->currentText())).value<Scalar*>())->value();
  } else {
    ok = false;
    ratio = ui->_scalar->currentText().toDouble(&ok);
    if (!ok) {
      ratio = Equations::interpret(_store, ui->_scalar->currentText().toLatin1(), &ok);
    }

    if (!ok) {
      ratio = 0;
    }
  }

  // set cutoff: either evaluate as equation, or convert to double.
  ok = false;
  cutoff = ui->_cutoff->text().toDouble(&ok);
  if (!ok) {
    cutoff = Equations::interpret(_store, ui->_cutoff->text().toLatin1(), &ok);
  }

  if (!ok) {
    cutoff = 0.0;
  }

  // set frequency: either evaluate as equation, or convert to double.
  ok = false;
  frequency = ui->_SR->text().toDouble(&ok);
  if (!ok) {
    frequency = Equations::interpret(_store, ui->_SR->text().toLatin1(), &ok);
  }

  if (!ok) {
    frequency = 0.0;
  }

  // keep physical
  if (cutoff <=0.0) {
    cutoff = 1.0;
  }

  if (frequency <= 0) {
    frequency = 1.0;
  }

  if (ratio <= 0) {
    ratio = 1.0;
  }

  if (control_field == Ratio) { // Frequency follows ratio: Keep SR fixed
    cutoff = ratio * frequency;
    ui->_cutoff->setText(QString::number(cutoff, 'g', 12));
  } else if (control_field == Cutoff) { // Ratio follows frequency: Keep SR fixed
    ratio = cutoff/frequency;
    setDefaultValue(ratio);
    //QString string = QString::number(ratio, 'g', 12);
    //_scalar->addItem(string, QVariant::fromValue(0));
    //_scalar->setCurrentIndex(_scalar->findText(string));
    //_scalar->setCurrentText(QString::number(ratio, 'g', 12));
  } else if (control_field == SampleRate) { // Cutoff follows SR.  Keep ratio fixed
    cutoff = ratio * frequency;
    ui->_cutoff->setText(QString::number(cutoff, 'g', 12));
    //ratio = cutoff/frequency;
    //_scalar->setCurrentText(QString::number(ratio, 'g', 12));
  }

}

}

// vim: ts=2 sw=2 et
