/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FFTOPTIONS_H
#define FFTOPTIONS_H

#include <QWidget>
#include "ui_fftoptions.h"

#include "psd.h"

#include "kst_export.h"

namespace Kst {

class FFTOptions : public QWidget, public Ui::FFTOptions {
  Q_OBJECT

public:
  FFTOptions(QWidget *parent = 0);
  ~FFTOptions();

  double sampleRate() const;
  double sigma() const;
  bool interleavedAverage() const;
  int FFTLength() const;
  bool apodize() const;
  bool removeMean() const;
  QString vectorUnits() const;
  QString rateUnits() const;
  ApodizeFunction apodizeFunction() const;
  PSDType output() const;
  bool interpolateOverHoles() const;

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
} KST_EXPORT;

}
#endif
// vim: ts=2 sw=2 et
