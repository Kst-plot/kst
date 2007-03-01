/***************************************************************************
                   kstdatarange.h
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

#ifndef KSTDATARANGE_H
#define KSTDATARANGE_H

#include <QWidget>
#include "ui_kstdatarange4.h"

class KstDataRange : public QWidget, public Ui::KstDataRange {
  Q_OBJECT

public:
  KstDataRange(QWidget *parent = 0);
  ~KstDataRange();

  void update();
  double f0Value();
  double nValue();
  KST::ExtDateTime f0DateTimeValue();
  bool isStartRelativeTime();
  bool isStartAbsoluteTime();
  bool isRangeRelativeTime();

public slots:
  void clickedCountFromEnd();
  void ClickedReadToEnd();
  void clickedDoSkip();
  void updateEnables();
  void setAllowTime(bool allow);
  void setF0Value(double v);
  void setNValue(double v);

private:
  void init();
  double interpret(const char *txt);

private:
  bool _time;
};

#endif
// vim: ts=2 sw=2 et
