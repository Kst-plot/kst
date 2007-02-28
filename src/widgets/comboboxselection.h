/***************************************************************************
                   comboboxselection.h
                             -------------------
    begin                :  2005
    copyright            : (C) 2005 The University of British Columbia
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

#ifndef COMBOBOXSELECTION_H
#define COMBOBOXSELECTION_H

#include <QDialog>
#include "ui_comboboxselection4.h"

class ComboBoxSelection : public QDialog, public Ui::ComboBoxSelection {
  Q_OBJECT

public:
  ComboBoxSelection(QWidget* parent = 0);
  virtual ~ComboBoxSelection();

  void addString(const QString &str);
  void reset();
  const QString& selected() const { return _selected; }

public slots:
  void ok();
  void changed();
  void sort();

private:
  QStringList _strs;
  QString _selected;
};

#endif
// vim: ts=2 sw=2 et
