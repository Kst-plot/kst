/***************************************************************************
                   akima_periodic.h
                             -------------------
    begin                : 12/13/06
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
#ifndef AKIMAPERIODIC_H
#define AKIMAPERIODIC_H

#include <kstbasicplugin.h>

class AkimaPeriodic : public KstBasicPlugin {
  Q_OBJECT
  public:
    AkimaPeriodic(QObject *parent, const char *name, const QStringList &args);
    virtual ~AkimaPeriodic();

    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;
};

#endif
