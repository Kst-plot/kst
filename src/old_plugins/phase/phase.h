/***************************************************************************
                   phase.h
                             -------------------
    begin                : 12/08/06
    copyright            : (C) 2006 The University of Toronto
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
#ifndef PHASE_H
#define PHASE_H

#include <kstbasicplugin.h>

class Phase : public KstBasicPlugin {
  Q_OBJECT
  public:
    Phase(QObject *parent, const char *name, const QStringList &args);
    virtual ~Phase();

    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;

  private:
    void swap(double* pData[], int iOne, int iTwo);
    void quicksort(double* pData[], int iLeft, int iRight);
};

#endif
