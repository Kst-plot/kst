/***************************************************************************
                   kstdatadialog.h
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

#ifndef KSTDATADIALOG_H
#define KSTDATADIALOG_H

#include <QDialog>
#include "ui_kstdatadialog4.h"

class KstDataDialog : public QDialog, public Ui::KstDataDialog {
  Q_OBJECT

public:
  KstDataDialog(QWidget *parent = 0);
  ~KstDataDialog();

public slots:
};

#endif
// vim: ts=2 sw=2 et
