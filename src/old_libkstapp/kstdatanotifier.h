/***************************************************************************
                             kstdatanotifier.h
                             -------------------
    begin                : Sep 13 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef KSTDATANOTIFIER_H
#define KSTDATANOTIFIER_H

#include <kled.h>

class KstDataNotifier : public KLed {
  Q_OBJECT
  public:
    KstDataNotifier(QWidget *parent);
    ~KstDataNotifier();

  public slots:
    void arrived();

  private slots:
    void animate();

  private:
    unsigned int _animationStage;
};


#endif

// vim: ts=2 sw=2 et
