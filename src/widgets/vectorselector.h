/***************************************************************************
                   vectorselector.h
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

#ifndef VECTORSELECTOR_H
#define VECTORSELECTOR_H

#include <QWidget>
#include "ui_vectorselector4.h"

class VectorSelector : public QWidget, public Ui::VectorSelector {
  Q_OBJECT

public:
  VectorSelector(QWidget *parent = 0);
  ~VectorSelector();

  QString selectedVector();

public slots:
  void allowNewVectors(bool allowed);
  void update();
  void createNewVector();
  void selectionWatcher(const QString &tag);
  void setSelection(const QString &tag);
  void newVectorCreated(KstVectorPtr v);
  void setSelection(KstVectorPtr v);
  void provideNoneVector(bool provide);
  void editVector();
  void setEdit(const QString &tag);

private:
  void init();

private:
  bool _provideNoneVector;
};

#endif
// vim: ts=2 sw=2 et
