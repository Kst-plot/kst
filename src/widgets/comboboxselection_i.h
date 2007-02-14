/**************************************************************************
        comboboxselection_i.h - source file: inherits designer dialog
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

#ifndef COMBOBOXSELECTIONI_H
#define COMBOBOXSELECTIONI_H

#include "comboboxselection.h"

class ComboBoxSelectionI : public ComboBoxSelection {
  Q_OBJECT
  public:
    ComboBoxSelectionI(QWidget* parent = 0, const char* name = 0,
        bool modal = FALSE, WFlags fl = 0 );
    virtual ~ComboBoxSelectionI();
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
  signals:
};

#endif
// vim: ts=2 sw=2 et
