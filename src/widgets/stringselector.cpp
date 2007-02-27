/***************************************************************************
                   stringselector.cpp
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "stringselector.h"

#include <QTimer>
#include <QLineEdit>
#include <QValidator>
#include <QListWidget>
#include <QAbstractItemView>

#include <kmessagebox.h>
#include <kiconloader.h>

#include <comboboxselection_i.h>
#include <kstdatacollection.h>
#include <kstcombobox.h>
#include <stringeditor.h>
#include <enodes.h>

#include <kst_export.h>

StringSelector::StringSelector(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);
}


StringSelector::~StringSelector() {}


void StringSelector::init() {
  update();
  _newString->setIcon(BarIcon("kst_stringnew"));
  _editString->setIcon(BarIcon("kst_stringedit"));
  connect(_selectString, SIGNAL(clicked()), this, SLOT(selectString()));
  connect(_newString, SIGNAL(clicked()), this, SLOT(createNewString()));
  connect(_editString, SIGNAL(clicked()), this, SLOT(editString()));
  connect(_string, SIGNAL(activated(const QString&)), this, SIGNAL(selectionChanged(const QString&)));
  connect(_string, SIGNAL(textChanged(const QString&)), this, SIGNAL(selectionChanged(const QString&)));
  connect(this, SIGNAL(selectionChanged(const QString&)), this, SLOT(selectionWatcher(const QString&)));
}


void StringSelector::allowNewStrings(bool allowed) {
  _newString->setEnabled(allowed);
}


void StringSelector::update() {
  if (_string->view()->isVisible()) {
    QTimer::singleShot(250, this, SLOT(update()));
    return;
  }

  blockSignals(true);

  QString prev = _string->currentText();
  bool found = false;
  QStringList strings;

  _string->clear();

  KST::stringList.lock().readLock();
  for (KstStringList::Iterator i = KST::stringList.begin(); i != KST::stringList.end(); ++i) {
    (*i)->readLock();
    QString tag = (*i)->tag().displayString();
    strings << tag;
    (*i)->unlock();

    if (tag == prev) {
      found = true;
    }
  }
  KST::stringList.lock().unlock();

  qSort(strings);
  _string->addItems(strings);
  if (found) {
    _string->setItemText(_string->currentIndex(), prev);
  }

  blockSignals(false);
}


void StringSelector::createNewString() {
  StringEditor *se = new StringEditor(this, "string editor");

  int rc = se->exec();
  if (rc == QDialog::Accepted) {
    bool ok = false;
    double val = se->_value->text().toFloat(&ok);

    if (!ok) {
      val = Equation::interpret(se->_value->text().toLatin1(), &ok);
    }

    if (ok) {
      KstStringPtr s = new KstString(KstObjectTag(se->_name->text(), KstObjectTag::globalTagContext), 0L);

      s->setOrphan(true);
      s->setEditable(true);
      emit newStringCreated();
      update();
      setSelection(s);
      _editString->setEnabled(true);
    } else {
      KMessageBox::sorry(this, tr("Error saving your new string."), tr("Kst"));
    }
  }

  delete se;
}


void StringSelector::selectString() {
  ComboBoxSelectionI *selection = new ComboBoxSelectionI(this, "string selector");
  int i;

  selection->reset();
  for (i=0; i<_string->count(); i++) {
    selection->addString(_string->itemText(i));
  }
  selection->sort();
  int rc = selection->exec();
  if (rc == QDialog::Accepted) {
    _string->setItemText(_string->currentIndex(), selection->selected());
  }

  delete selection;
}


void StringSelector::editString() {
  StringEditor *se = new StringEditor(this, "string editor");

  KstStringPtr pold = *KST::stringList.findTag(_string->currentText());
  if (pold && pold->editable()) {
    se->_value->setText(pold->value());
    se->_name->setText(pold->tag().tagString());
    se->_value->selectAll();
    se->_value->setFocus();
  }

  int rc = se->exec();
  if (rc == QDialog::Accepted) {
    bool ok = false;
    QString val = se->_value->text();

    if (ok) {
      KstStringPtr p = *KST::stringList.findTag(se->_name->text());
      if (p) {
        p->setValue(val);
        setSelection(p);
      } else {
        p = new KstString(KstObjectTag(se->_name->text(), KstObjectTag::globalTagContext), 0L, val);

        p->setOrphan(true);
        p->setEditable(true);
        emit newStringCreated();
        update();
        setSelection(p);
        _editString->setEnabled(true);
      }
    } else {
      KMessageBox::sorry(this, tr("Error saving your new string."), tr("Kst"));
    }
  }

  delete se;
}


void StringSelector::selectionWatcher(const QString &tag) {
  bool editable = false;

  QString label = "["+tag+"]";
  emit selectionChangedLabel(label);
  KST::stringList.lock().readLock();
  KstStringPtr p = *KST::stringList.findTag(tag);
  if (p && p->editable()) {
    editable = true;
  }
  KST::stringList.lock().unlock();
  _editString->setEnabled(editable);
}


void StringSelector::setSelection(const QString &tag) {
  if (tag.isEmpty()) {
    return;
  }
  blockSignals(true);
  _string->setItemText(_string->currentIndex(), tag);
  selectionWatcher(tag);
  blockSignals(false);
}


void StringSelector::setSelection(KstStringPtr s) {
  setSelection(s->tagName());
}


QString StringSelector::selectedString() {
  return _string->currentText();
}


void StringSelector::allowDirectEntry(bool allowed) {
  _string->setEditable(allowed);
}

#include "stringselector.moc"

// vim: ts=2 sw=2 et
