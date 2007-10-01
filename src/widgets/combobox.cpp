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

#include "combobox.h"
#include <QLineEdit>
#include <QCompleter>

namespace Kst {

ComboBox::ComboBox(QWidget *parent)
  : QComboBox(parent), _editable(true) {
  setupLineEdit();
}


ComboBox::ComboBox(bool editable, QWidget *parent)
  : QComboBox(parent), _editable(editable) {
  setupLineEdit();
}

void ComboBox::setupLineEdit() {
  setInsertPolicy(QComboBox::NoInsert);

  setLineEdit(new QLineEdit(this));

  QCompleter *currentCompleter = completer();
  currentCompleter->setCompletionMode(QCompleter::PopupCompletion);
}


ComboBox::~ComboBox() {
}


void ComboBox::setEditable(bool editable) {
  _editable = editable;
}


}

// vim: ts=2 sw=2 et
