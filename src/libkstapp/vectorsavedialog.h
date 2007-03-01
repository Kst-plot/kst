/***************************************************************************
                   vectorsavedialog.h
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

#ifndef VECTORSAVEDIALOG_H
#define VECTORSAVEDIALOG_H

#include <QWidget>
#include "ui_vectorsavedialog4.h"

class VectorSaveDialog : public QWidget, public Ui::VectorSaveDialog {
  Q_OBJECT

public:
  VectorSaveDialog(QWidget *parent = 0);
  ~VectorSaveDialog();

public slots:
  void show();
  void init();

private slots:
  void selectionChanged();
  void save();
};

#endif
// vim: ts=2 sw=2 et
