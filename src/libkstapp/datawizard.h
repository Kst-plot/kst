/***************************************************************************
                   datawizard.h
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

#ifndef DATAWIZARD_H
#define DATAWIZARD_H

#include <QWidget>
#include "ui_datawizard4.h"

class DataWizard : public QWidget, public Ui::DataWizard {
  Q_OBJECT

public:
  DataWizard(QWidget *parent = 0);
  ~DataWizard();

public slots:
};

#endif
// vim: ts=2 sw=2 et
