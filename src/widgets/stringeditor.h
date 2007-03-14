/***************************************************************************
                   stringeditor.h
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

#ifndef STRINGEDITOR_H
#define STRINGEDITOR_H

#include <QDialog>
#include "ui_stringeditor4.h"

#include "kst_export.h"

class StringEditor : public QDialog, public Ui::StringEditor {
  Q_OBJECT

public:
  StringEditor(QWidget *parent = 0);
  ~StringEditor();
};

#endif
// vim: ts=2 sw=2 et

