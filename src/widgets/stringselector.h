/***************************************************************************
                   stringselector.h
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

#ifndef STRINGSELECTOR_H
#define STRINGSELECTOR_H

#include <QWidget>
#include "ui_stringselector4.h"

#include <kstdataobject.h>

#include "kst_export.h"

class StringSelector : public QWidget, public Ui::StringSelector {
  Q_OBJECT

public:
  StringSelector(QWidget *parent = 0);
  ~StringSelector();

  QString selectedString();

public slots:
  void allowNewStrings(bool allowed);
  void update();
  void createNewString();
  void selectString();
  void editString();
  void selectionWatcher(const QString &tag);
  void setSelection(const QString &tag);
  void setSelection(KstStringPtr s);
  void allowDirectEntry(bool allowed);

signals:
  void newStringCreated();
  void selectionChanged(const QString &);
  void selectionChangedLabel(const QString &);

private:
  void init();
} KST_EXPORT;

#endif
// vim: ts=2 sw=2 et
