/***************************************************************************
                   scalarselector.h
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

#ifndef SCALARSELECTOR_H
#define SCALARSELECTOR_H

#include <QWidget>
#include "ui_scalarselector4.h"

class ScalarSelector : public QWidget, public Ui::ScalarSelector {
  Q_OBJECT

public:
  ScalarSelector(QWidget *parent = 0);
  ~ScalarSelector();

  QString selectedScalar();

public slots:
  void allowNewScalars(bool allowed);
  void update();
  void createNewScalar();
  void selectScalar();
  void editScalar();
  void selectionWatcher(const QString &tag);
  void setSelection(const QString &tag);
  void setSelection(KstScalarPtr s);
  void allowDirectEntry(bool allowed);

private:
  void init();
};

#endif
// vim: ts=2 sw=2 et
