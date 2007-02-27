/***************************************************************************
                               kstcombobox.cpp
                             -------------------
    begin                : 12/14/06
    copyright            : (C) 2006 The University of Toronto
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

#include "kstcombobox.h"
//Added by qt3to4:
#include <QFocusEvent>

KstComboBox::KstComboBox(QWidget *parent, const char *name)
  : KComboBox(parent, name), _trueRW(false) {

  commonConstructor();
}


KstComboBox::KstComboBox(bool rw, QWidget *parent, const char *name)
  : KComboBox(false, parent, name), _trueRW(rw) {

  commonConstructor();
}


KstComboBox::~KstComboBox() {
}

void KstComboBox::setEditable(bool rw) {
  _trueRW = rw;
}

void KstComboBox::commonConstructor() {

  QComboBox::setEditable( true );

  if (!_trueRW) { //if not truly read write then go into psuedo mode for read only
    setInsertionPolicy( NoInsertion );
    setCompletionMode( KGlobalSettings::CompletionPopupAuto );

    //DON'T HANDLE THE EDIT'S RETURNPRESSED IN qcombobox.cpp... RATHER HANDLE HERE!
    disconnect( lineEdit(), SIGNAL(returnPressed()), this, SLOT(returnPressed()) );
    connect( this, SIGNAL(returnPressed()), this, SLOT(validate()) );
  }
}

void KstComboBox::focusInEvent(QFocusEvent *event) {
  //WARNING!! If the list of items changes programmatically while the combo has focus
  //this will bug out!!  Unfortunately I see no way to check whether the list of items
  //changes programmatically other than to provide my own input methods or poll with a
  //timer.  Neither is a good idea IMO...

  if (!_trueRW) {
    if (KCompletion *comp = completionObject()) {
      comp->clear();
      for (int i = 0; i < count(); ++i) {
        comp->addItem(text(i));
      }
    }
  }

  KComboBox::focusInEvent(event);
}

void KstComboBox::focusOutEvent(QFocusEvent *event) {
  //WARNING!! If the list of items changes programmatically while the combo has focus
  //this will bug out!!  Unfortunately I see no way to check whether the list of items
  //changes programmatically other than to provide my own input methods or poll with a
  //timer.  Neither is a good idea IMO...

  validate(false);

  KComboBox::focusOutEvent(event);
}


void KstComboBox::validate(bool rp) {
  if (!_trueRW) {
    int match = -1;
    for (int i = 0; i < count(); ++i) {
      match = currentText() == text(i) ? i : match;
    }

    if (match < 0 && count()) {
      lineEdit()->blockSignals(true);
      lineEdit()->setText( text(currentItem()) );
      lineEdit()->blockSignals(false);

    } else if (match != currentItem() || rp) {
      setCurrentItem(match);

      emit activated(match);
      emit activated(text(match));
    }
  }
}


#include "kstcombobox.moc"
// vim: ts=2 sw=2 et
