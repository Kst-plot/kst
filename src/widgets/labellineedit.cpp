/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labellineedit.h"

#include <QDebug>

namespace Kst {

LabelLineEdit::LabelLineEdit(QWidget *parent)
  : QLineEdit(parent) {
}


LabelLineEdit::~LabelLineEdit() {
}

void LabelLineEdit::focusInEvent ( QFocusEvent * event ) {
  emit inFocus();
  QLineEdit::focusInEvent(event);
}

}

// vim: ts=2 sw=2 et
