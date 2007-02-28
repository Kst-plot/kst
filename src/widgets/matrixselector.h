/***************************************************************************
                   matrixselector.h
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

#ifndef MATRIXSELECTOR_H
#define MATRIXSELECTOR_H

#include <QWidget>
#include "ui_matrixselector4.h"

class MatrixSelector : public QWidget, public Ui::MatrixSelector {
  Q_OBJECT

public:
  MatrixSelector(QWidget *parent = 0);
  ~MatrixSelector();

  QString selectedMatrix();

public slots:
  void allowNewMatrices(bool allowed);
  void update();
  void createNewMatrix();
  void selectionWatcher(const QString &tag);
  void setSelection(const QString &tag);
  void newMatrixCreated(KstMatrixPtr v);
  void setSelection(KstMatrixPtr v);
  void provideNoneMatrix(bool provide);
  void editMatrix();
  void setEdit(const QString &tag);

signals:
  void newMatrixCreated(const QString &);
  void selectionChanged(const QString &);
  void selectionChangedLabel(const QString &label);

private:
  void init();

private:
  bool _provideNoneMatrix;
};

#endif
// vim: ts=2 sw=2 et
