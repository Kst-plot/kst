/***************************************************************************
                         dmcobj.h  -  Part of KST
                             -------------------
    begin                : Mon Oct 06 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KST_DMOBJECT_H
#define KST_DMOBJECT_H

#include "dmcdata.h"

#include <QMap>


namespace DMC
{
    enum TimeType { Undetermined = -1, Unknown = 0, TimeSec = 1, TimesOfSamples = 2 };

class ObjectGroup;

class Object : public Source
{
  public:
    Object();
    virtual ~Object();

    virtual bool setGroup(const QString& group);
 
    virtual QStringList fields() const;

    virtual void reset();

    virtual bool updated() const;

    virtual int readObject(const QString& object, double *buf, long start, long end);

    // FIXME: QSize is a bogus class to use here
    virtual QSize range(const QString& object) const;

    TimeType typeOfTime();
    double fetchTimePoint(PIOLONG);

    int sampleForTime(double ms, bool *ok);
    int sampleForTimeRelative(double ms, bool *ok);
    double timeForSample(int sample);

  private:
    // We lazy load the groups, hence mutable
    mutable QMap<QString, ObjectGroup*> _groupInfo;
    ObjectGroup *findGroup(const QString& group) const;
    TimeType _tt;
};

}

#endif

