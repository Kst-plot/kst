/***************************************************************************
                   kstsettingsdlg.h
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

#ifndef KSTSETTINGSDLG_H
#define KSTSETTINGSDLG_H

#include <QWidget>
#include "ui_kstsettingsdlg4.h"

class KstSettingsDlg : public QWidget, public Ui::KstSettingsDlg {
  Q_OBJECT

public:
  KstSettingsDlg(QWidget *parent = 0);
  ~KstSettingsDlg();

public slots:
};

#endif
// vim: ts=2 sw=2 et
