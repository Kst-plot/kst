/***************************************************************************
                   extensiondlg.h
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

#ifndef EXTENSIONDIALOG_H
#define EXTENSIONDIALOG_H

#include <QWidget>
#include "ui_extensiondlg4.h"

class ExtensionDialog : public QWidget, public Ui::ExtensionDialog {
  Q_OBJECT

public:
  ExtensionDialog(QWidget *parent = 0);
  ~ExtensionDialog();

public slots:
  void show();
  void accept();
};

#endif
// vim: ts=2 sw=2 et
