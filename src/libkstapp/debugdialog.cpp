/***************************************************************************
                   debugdialog.cpp
                             -------------------
    begin                : 02/28/07
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

#include "debugdialog.h"

#include <kst_export.h>

DebugDialog::DebugDialog(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(, SIGNAL(), this, SLOT());
}


DebugDialog::~DebugDialog() {}


void DebugDialog::init() {
  _dataSources->setAllColumnsShowFocus(true);
}

#include "debugdialog.moc"

// vim: ts=2 sw=2 et
