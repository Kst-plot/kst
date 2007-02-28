/***************************************************************************
                   stringeditor.cpp
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

#include "stringeditor.h"

#include <kst_export.h>

StringEditor::StringEditor(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

  connect(pushButton10, SIGNAL(clicked()), this, SLOT(reject()));
  connect(pushButton11, SIGNAL(clicked()), this, SLOT(accept()));
}


StringEditor::~StringEditor() {}

#include "stringeditor.moc"

// vim: ts=2 sw=2 et
