/***************************************************************************
                   kstfftoptions.h
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

#ifndef KSTFFTOPTIONS_H
#define KSTFFTOPTIONS_H

#include <QWidget>
#include "ui_kstfftoptions4.h"

class KstFFTOptions : public QWidget, public Ui::KstFFTOptions {
  Q_OBJECT

public:
  KstFFTOptions(QWidget *parent = 0);
  ~KstFFTOptions();

  void update();

public slots:
  void changedApodizeFxn();
  void clickedInterleaved();
  void clickedApodize();
  void synch();
  bool checkValues();
  bool checkGivenValues(double sampRate, int FFTLen);

private:
  void init();
};

#endif
// vim: ts=2 sw=2 et
