/***************************************************************************
                   scalareditor.h
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

#ifndef SCALAREDITOR_H
#define SCALAREDITOR_H

#include <QDialog>
#include "ui_scalareditor4.h"

class ScalarEditor : public QDialog, public Ui::ScalarEditor {
  Q_OBJECT

public:
  ScalarEditor(QWidget *parent = 0);
  ~ScalarEditor();
};

#endif
// vim: ts=2 sw=2 et

