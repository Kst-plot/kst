/*
    Copyright (c) 2005 The University of Toronto
    Based on code Copyright (C) 2005, S.R.Haque <srhaque@iee.org>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KTIMEZONECOMBO_H
#define KTIMEZONECOMBO_H

#include <kcombobox.h>
#include <qstring.h>

class KstTimezones;

class KTimezoneCombo : public KComboBox {
  Q_OBJECT
  public:
    KTimezoneCombo(QWidget *parent = 0L, KstTimezones *db = 0L);
    virtual ~KTimezoneCombo();

    // returns the offset from UTC in seconds
    int offset() const;
    QString tzName() const;

    void setTimezone(const QString& tz);

  private:
    class Private;
    Private *d;
};

#endif
// vim: ts=2 sw=2 et
