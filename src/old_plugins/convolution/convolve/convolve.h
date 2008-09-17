/***************************************************************************
                   convolve.h
                             -------------------
    begin                : 11/29/06
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
#ifndef CONVOLVE_H
#define CONVOLVE_H

#include <kstbasicplugin.h>

class Convolve : public KstBasicPlugin {
  Q_OBJECT
  public:
    Convolve(QObject *parent, const char *name, const QStringList &args);
    virtual ~Convolve();

    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;
};

#endif
