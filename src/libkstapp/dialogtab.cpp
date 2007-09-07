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

#include "dialogtab.h"

namespace Kst {

DialogTab::DialogTab(QWidget *parent)
  : QWidget(parent) {
}


DialogTab::~DialogTab() {
}


void DialogTab::apply() {
  emit modified(false);
}


void DialogTab::restoreDefaults() {
  emit modified(false);
}


void DialogTab::showEvent(QShowEvent *event) {
  restoreDefaults();
  QWidget::showEvent(event);
}

}

// vim: ts=2 sw=2 et
